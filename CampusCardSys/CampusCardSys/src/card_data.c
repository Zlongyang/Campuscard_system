/**
 * card_data.c — 核心数据操作层实现
 * 所有业务数据读写都在此层进行，上层 API 通过此层操作数据
 */
#include "include/card_data.h"
#include "include/file_io.h"
#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== 内部：ID 管理 ========== */

static int g_next_student_id = 1;
static int g_next_card_id = 1;
static int g_next_record_id = 1;
static int g_ids_loaded = 0;

static void ensure_ids_loaded(void) {
    if (!g_ids_loaded) {
        file_ensure_all();
        file_load_next_ids(&g_next_student_id, &g_next_card_id, &g_next_record_id);
        g_ids_loaded = 1;
    }
}

static void ids_persist(void) {
    file_save_next_ids(g_next_student_id, g_next_card_id, g_next_record_id);
}

int get_next_student_id(void) {
    ensure_ids_loaded();
    int id = g_next_student_id++;
    ids_persist();
    return id;
}

int get_next_card_id(void) {
    ensure_ids_loaded();
    int id = g_next_card_id++;
    ids_persist();
    return id;
}

int get_next_record_id(void) {
    ensure_ids_loaded();
    int id = g_next_record_id++;
    ids_persist();
    return id;
}

/* ========== 学生管理 ========== */

int student_add(const Student *s) {
    if (!s) return -1;
    ensure_ids_loaded();

    /* 检查学号是否已存在 */
    Student existing;
    if (student_find_by_no(s->student_no, &existing) == 0) {
        return -2; /* 学号已存在 */
    }

    /* 分配 ID 并写入 */
    Student new_student = *s;
    new_student.id = get_next_student_id();
    return file_append_student(&new_student);
}

int student_update(const Student *s) {
    if (!s) return -1;

    /* 加载所有学生 */
    Student students[MAX_STUDENTS];
    int count = file_load_students(students, MAX_STUDENTS);

    /* 查找并更新 */
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].student_no, s->student_no) == 0) {
            /* 保留 ID */
            int old_id = students[i].id;
            students[i] = *s;
            students[i].id = old_id;
            found = 1;
            break;
        }
    }

    if (!found) return -2; /* 学生不存在 */

    return file_save_students(students, count);
}

int student_delete(const char *student_no) {
    if (!student_no) return -1;

    Student students[MAX_STUDENTS];
    int count = file_load_students(students, MAX_STUDENTS);

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].student_no, student_no) == 0) {
            /* 将后面的元素前移 */
            for (int j = i; j < count - 1; j++) {
                students[j] = students[j + 1];
            }
            count--;
            found = 1;
            break;
        }
    }

    if (!found) return -2;
    return file_save_students(students, count);
}

int student_find_by_no(const char *student_no, Student *out) {
    if (!student_no || !out) return -1;

    Student students[MAX_STUDENTS];
    int count = file_load_students(students, MAX_STUDENTS);

    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].student_no, student_no) == 0) {
            *out = students[i];
            return 0;
        }
    }
    return -2; /* 未找到 */
}

int student_find_all(Student *out, int max_count) {
    if (!out) return -1;
    return file_load_students(out, max_count);
}

int student_search(const char *keyword, Student *out, int max_count) {
    if (!keyword || !out) return -1;

    Student all[MAX_STUDENTS];
    int total = file_load_students(all, MAX_STUDENTS);
    int found = 0;

    char kw_lower[MAX_FIELD_LEN];
    str_safe_copy(kw_lower, keyword, sizeof(kw_lower));
    str_tolower(kw_lower);

    for (int i = 0; i < total && found < max_count; i++) {
        char no_lower[32], name_lower[MAX_NAME_LEN], dept_lower[MAX_DEPT_LEN];
        str_safe_copy(no_lower, all[i].student_no, sizeof(no_lower));
        str_safe_copy(name_lower, all[i].name, sizeof(name_lower));
        str_safe_copy(dept_lower, all[i].department, sizeof(dept_lower));
        str_tolower(no_lower);
        str_tolower(name_lower);
        str_tolower(dept_lower);

        if (strstr(no_lower, kw_lower) || strstr(name_lower, kw_lower) || strstr(dept_lower, kw_lower)) {
            out[found++] = all[i];
        }
    }
    return found;
}

/* ========== 内部：卡状态修改 ========== */

static int card_status_change(const char *card_no, int new_status) {
    if (!card_no) return -1;

    Card cards[MAX_CARDS];
    int count = file_load_cards(cards, MAX_CARDS);
    int found = 0;

    for (int i = 0; i < count; i++) {
        if (strcmp(cards[i].card_no, card_no) == 0) {
            /* 状态流转校验 */
            int cur = cards[i].status;

            /* 已注销的卡不能操作 */
            if (cur == CARD_CANCELLED) return -4;
            /* 相同状态不操作 */
            if (cur == new_status) return -5;

            cards[i].status = new_status;
            found = 1;
            break;
        }
    }

    if (!found) return -2;
    return file_save_cards(cards, count);
}

/* ========== 校园卡管理 ========== */

int card_issue(const Card *c) {
    if (!c) return -1;
    ensure_ids_loaded();

    /* 检查卡号是否已存在 */
    Card existing;
    if (card_find_by_no(c->card_no, &existing) == 0) {
        return -2; /* 卡号已存在 */
    }

    /* 检查该学生是否已有有效卡 */
    Card stu_card;
    if (card_find_by_student(c->student_no, &stu_card) == 0) {
        if (stu_card.status == CARD_ACTIVE || stu_card.status == CARD_FROZEN) {
            return -3; /* 已有一张有效卡 */
        }
    }

    Card new_card = *c;
    new_card.id = get_next_card_id();
    return file_append_card(&new_card);
}

int card_freeze(const char *card_no) {
    return card_status_change(card_no, CARD_FROZEN);
}

int card_unfreeze(const char *card_no) {
    return card_status_change(card_no, CARD_ACTIVE);
}

int card_report_lost(const char *card_no) {
    return card_status_change(card_no, CARD_LOST);
}

int card_cancel(const char *card_no) {
    return card_status_change(card_no, CARD_CANCELLED);
}

int card_recharge(const char *card_no, double amount, const char *desc) {
    if (!card_no || !validate_amount(amount)) return -1;

    Card cards[MAX_CARDS];
    int count = file_load_cards(cards, MAX_CARDS);
    int found = 0;
    int idx = -1;

    for (int i = 0; i < count; i++) {
        if (strcmp(cards[i].card_no, card_no) == 0) {
            if (cards[i].status != CARD_ACTIVE) {
                return -3; /* 卡状态不正常 */
            }
            idx = i;
            found = 1;
            break;
        }
    }

    if (!found) return -2;

    /* 更新余额 */
    double balance_before = cards[idx].balance;
    cards[idx].balance += amount;

    /* 写回卡文件 */
    if (file_save_cards(cards, count) != count) {
        return -6; /* 写入失败 */
    }

    /* 添加交易记录 */
    Record r;
    memset(&r, 0, sizeof(r));
    r.id = get_next_record_id();
    str_safe_copy(r.card_no, card_no, sizeof(r.card_no));
    str_safe_copy(r.student_no, cards[idx].student_no, sizeof(r.student_no));
    r.type = REC_RECHARGE;
    r.amount = amount;
    r.balance_before = balance_before;
    r.balance_after = cards[idx].balance;
    time_now_str(r.time_str, sizeof(r.time_str));
    str_safe_copy(r.description, desc ? desc : "充值", sizeof(r.description));
    file_append_record(&r);

    return 0;
}

int card_consume(const char *card_no, double amount, const char *desc) {
    if (!card_no || !validate_amount(amount)) return -1;

    Card cards[MAX_CARDS];
    int count = file_load_cards(cards, MAX_CARDS);
    int found = 0;
    int idx = -1;

    for (int i = 0; i < count; i++) {
        if (strcmp(cards[i].card_no, card_no) == 0) {
            if (cards[i].status != CARD_ACTIVE) {
                return -3; /* 卡状态不正常 */
            }
            idx = i;
            found = 1;
            break;
        }
    }

    if (!found) return -2;

    /* 检查余额 */
    if (cards[idx].balance < amount) {
        return -4; /* 余额不足 */
    }

    /* 检查当日消费限额 */
    double today_spent = 0;
    char today[20];
    date_now_str(today, sizeof(today));
    Record records[MAX_RECORDS];
    int rcount = file_load_records(records, MAX_RECORDS);
    for (int i = 0; i < rcount; i++) {
        if (strcmp(records[i].card_no, card_no) == 0 &&
            records[i].type == REC_CONSUME &&
            strncmp(records[i].time_str, today, 10) == 0) {
            today_spent += records[i].amount;
        }
    }
    if (today_spent + amount > cards[idx].daily_limit) {
        return -5; /* 超过每日限额 */
    }

    /* 扣款 */
    double balance_before = cards[idx].balance;
    cards[idx].balance -= amount;

    if (file_save_cards(cards, count) != count) {
        return -6;
    }

    /* 添加交易记录 */
    Record r;
    memset(&r, 0, sizeof(r));
    r.id = get_next_record_id();
    str_safe_copy(r.card_no, card_no, sizeof(r.card_no));
    str_safe_copy(r.student_no, cards[idx].student_no, sizeof(r.student_no));
    r.type = REC_CONSUME;
    r.amount = amount;
    r.balance_before = balance_before;
    r.balance_after = cards[idx].balance;
    time_now_str(r.time_str, sizeof(r.time_str));
    str_safe_copy(r.description, desc ? desc : "消费", sizeof(r.description));
    file_append_record(&r);

    return 0;
}

int card_find_by_no(const char *card_no, Card *out) {
    if (!card_no || !out) return -1;

    Card cards[MAX_CARDS];
    int count = file_load_cards(cards, MAX_CARDS);

    for (int i = 0; i < count; i++) {
        if (strcmp(cards[i].card_no, card_no) == 0) {
            *out = cards[i];
            return 0;
        }
    }
    return -2;
}

int card_find_by_student(const char *student_no, Card *out) {
    if (!student_no || !out) return -1;

    Card cards[MAX_CARDS];
    int count = file_load_cards(cards, MAX_CARDS);

    /* 返回最新的有效卡 */
    for (int i = count - 1; i >= 0; i--) {
        if (strcmp(cards[i].student_no, student_no) == 0) {
            /* 优先返回正常或冻结状态的卡 */
            if (cards[i].status == CARD_ACTIVE || cards[i].status == CARD_FROZEN || cards[i].status == CARD_LOST) {
                *out = cards[i];
                return 0;
            }
        }
    }
    /* 如果没有有效卡，返回最新的一张（可能是已注销的） */
    for (int i = count - 1; i >= 0; i--) {
        if (strcmp(cards[i].student_no, student_no) == 0) {
            *out = cards[i];
            return 0;
        }
    }
    return -2;
}

int card_find_all(Card *out, int max_count) {
    if (!out) return -1;
    return file_load_cards(out, max_count);
}

int card_search(const char *keyword, Card *out, int max_count) {
    if (!keyword || !out) return -1;

    Card all[MAX_CARDS];
    int total = file_load_cards(all, MAX_CARDS);
    int found = 0;

    char kw_lower[MAX_FIELD_LEN];
    str_safe_copy(kw_lower, keyword, sizeof(kw_lower));
    str_tolower(kw_lower);

    for (int i = 0; i < total && found < max_count; i++) {
        char no_lower[32], stu_lower[32], name_lower[MAX_NAME_LEN];
        str_safe_copy(no_lower, all[i].card_no, sizeof(no_lower));
        str_safe_copy(stu_lower, all[i].student_no, sizeof(stu_lower));
        str_safe_copy(name_lower, all[i].name, sizeof(name_lower));
        str_tolower(no_lower);
        str_tolower(stu_lower);
        str_tolower(name_lower);

        if (strstr(no_lower, kw_lower) || strstr(stu_lower, kw_lower) || strstr(name_lower, kw_lower)) {
            out[found++] = all[i];
        }
    }
    return found;
}

/* ========== 交易记录 ========== */

int record_add(const Record *r) {
    if (!r) return -1;
    ensure_ids_loaded();

    Record new_rec = *r;
    new_rec.id = get_next_record_id();
    return file_append_record(&new_rec);
}

int record_find_by_card(const char *card_no, Record *out, int max_count) {
    if (!card_no || !out) return -1;

    Record all[MAX_RECORDS];
    int total = file_load_records(all, MAX_RECORDS);
    int found = 0;

    for (int i = total - 1; i >= 0 && found < max_count; i--) {
        if (strcmp(all[i].card_no, card_no) == 0) {
            out[found++] = all[i];
        }
    }
    return found;
}

int record_find_by_student(const char *student_no, Record *out, int max_count) {
    if (!student_no || !out) return -1;

    Record all[MAX_RECORDS];
    int total = file_load_records(all, MAX_RECORDS);
    int found = 0;

    for (int i = total - 1; i >= 0 && found < max_count; i--) {
        if (strcmp(all[i].student_no, student_no) == 0) {
            out[found++] = all[i];
        }
    }
    return found;
}

int record_find_all(Record *out, int max_count) {
    if (!out) return -1;
    return file_load_records(out, max_count);
}

int record_search(const char *keyword, Record *out, int max_count) {
    if (!keyword || !out) return -1;

    Record all[MAX_RECORDS];
    int total = file_load_records(all, MAX_RECORDS);
    int found = 0;

    char kw_lower[MAX_FIELD_LEN];
    str_safe_copy(kw_lower, keyword, sizeof(kw_lower));
    str_tolower(kw_lower);

    for (int i = total - 1; i >= 0 && found < max_count; i--) {
        char no_lower[32], desc_lower[MAX_DESC_LEN];
        str_safe_copy(no_lower, all[i].card_no, sizeof(no_lower));
        str_safe_copy(desc_lower, all[i].description, sizeof(desc_lower));
        str_tolower(no_lower);
        str_tolower(desc_lower);

        if (strstr(no_lower, kw_lower) || strstr(desc_lower, kw_lower)) {
            out[found++] = all[i];
        }
    }
    return found;
}

/* ========== 统计 ========== */

int stats_get(SystemStats *stats) {
    if (!stats) return -1;
    memset(stats, 0, sizeof(SystemStats));

    /* 统计学生 */
    Student students[MAX_STUDENTS];
    stats->total_students = file_load_students(students, MAX_STUDENTS);

    /* 统计卡 */
    Card cards[MAX_CARDS];
    stats->total_cards = file_load_cards(cards, MAX_CARDS);

    for (int i = 0; i < stats->total_cards; i++) {
        switch (cards[i].status) {
            case CARD_ACTIVE:    stats->active_cards++;    break;
            case CARD_FROZEN:    stats->frozen_cards++;    break;
            case CARD_LOST:      stats->lost_cards++;      break;
            case CARD_CANCELLED: stats->cancelled_cards++; break;
        }
        if (cards[i].status == CARD_ACTIVE) {
            stats->total_balance += cards[i].balance;
        }
    }

    /* 统计今日交易 */
    char today[20];
    date_now_str(today, sizeof(today));

    Record records[MAX_RECORDS];
    int rcount = file_load_records(records, MAX_RECORDS);
    for (int i = 0; i < rcount; i++) {
        if (strncmp(records[i].time_str, today, 10) == 0) {
            if (records[i].type == REC_RECHARGE) {
                stats->today_recharges++;
                stats->today_recharge_amount += records[i].amount;
            } else if (records[i].type == REC_CONSUME) {
                stats->today_consumes++;
                stats->today_consume_amount += records[i].amount;
            }
        }
    }

    return 0;
}
