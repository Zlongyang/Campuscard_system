/**
 * admin_op.c — 管理员操作 API 实现
 * 每个函数接收参数，调用数据层，返回 JSON 结果
 */
#include "include/admin_op.h"
#include "include/file_io.h"
#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== 内部辅助 ========== */

/* 将 Student 对象转为 JSON 对象字符串 */
static void student_to_json(const Student *s, char *out, size_t size) {
    snprintf(out, size,
             "{\"id\":%d,\"student_no\":\"%s\",\"name\":\"%s\","
             "\"department\":\"%s\",\"grade\":\"%s\",\"phone\":\"%s\"}",
             s->id, s->student_no, s->name,
             s->department, s->grade, s->phone);
}

/* 将 Student 数组转为 JSON 数组字符串 */
static void students_to_json(const Student *list, int count, char *out, size_t size) {
    if (count <= 0) {
        snprintf(out, size, "[]");
        return;
    }

    size_t pos = 0;
    pos += snprintf(out + pos, size - pos, "[");
    for (int i = 0; i < count; i++) {
        char item[512];
        student_to_json(&list[i], item, sizeof(item));
        if (i > 0) pos += snprintf(out + pos, size - pos, ",");
        pos += snprintf(out + pos, size - pos, "%s", item);
    }
    pos += snprintf(out + pos, size - pos, "]");
}

/* 将 Card 对象转为 JSON */
static void card_to_json(const Card *c, char *out, size_t size) {
    const char *status_str;
    switch (c->status) {
        case CARD_ACTIVE:    status_str = "正常"; break;
        case CARD_FROZEN:    status_str = "冻结"; break;
        case CARD_LOST:      status_str = "挂失"; break;
        case CARD_CANCELLED: status_str = "注销"; break;
        default:             status_str = "未知"; break;
    }
    snprintf(out, size,
             "{\"id\":%d,\"card_no\":\"%s\",\"student_no\":\"%s\",\"name\":\"%s\","
             "\"balance\":%.2f,\"status\":%d,\"status_str\":\"%s\","
             "\"daily_limit\":%.2f,\"issue_date\":\"%s\"}",
             c->id, c->card_no, c->student_no, c->name,
             c->balance, c->status, status_str,
             c->daily_limit, c->issue_date);
}

/* 将 Card 数组转为 JSON */
static void cards_to_json(const Card *list, int count, char *out, size_t size) {
    if (count <= 0) {
        snprintf(out, size, "[]");
        return;
    }

    size_t pos = 0;
    pos += snprintf(out + pos, size - pos, "[");
    for (int i = 0; i < count; i++) {
        char item[512];
        card_to_json(&list[i], item, sizeof(item));
        if (i > 0) pos += snprintf(out + pos, size - pos, ",");
        pos += snprintf(out + pos, size - pos, "%s", item);
    }
    pos += snprintf(out + pos, size - pos, "]");
}

/* 将 Record 对象转为 JSON */
static void record_to_json(const Record *r, char *out, size_t size) {
    const char *type_str;
    switch (r->type) {
        case REC_RECHARGE: type_str = "充值"; break;
        case REC_CONSUME:  type_str = "消费"; break;
        case REC_REFUND:   type_str = "退款"; break;
        default:           type_str = "未知"; break;
    }
    snprintf(out, size,
             "{\"id\":%d,\"card_no\":\"%s\",\"student_no\":\"%s\","
             "\"type\":%d,\"type_str\":\"%s\","
             "\"amount\":%.2f,\"balance_before\":%.2f,\"balance_after\":%.2f,"
             "\"time\":\"%s\",\"description\":\"%s\"}",
             r->id, r->card_no, r->student_no,
             r->type, type_str,
             r->amount, r->balance_before, r->balance_after,
             r->time_str, r->description);
}

static void records_to_json(const Record *list, int count, char *out, size_t size) {
    if (count <= 0) {
        snprintf(out, size, "[]");
        return;
    }

    size_t pos = 0;
    pos += snprintf(out + pos, size - pos, "[");
    for (int i = 0; i < count; i++) {
        char item[768];
        record_to_json(&list[i], item, sizeof(item));
        if (i > 0) pos += snprintf(out + pos, size - pos, ",");
        pos += snprintf(out + pos, size - pos, "%s", item);
    }
    pos += snprintf(out + pos, size - pos, "]");
}

/* ========== 学生管理 ========== */

int admin_student_list(char *json_out, size_t size) {
    Student list[MAX_STUDENTS];
    int count = student_find_all(list, MAX_STUDENTS);

    char data[65536];
    students_to_json(list, count, data, sizeof(data));
    json_build_resp(json_out, size, 200, "ok", data);
    return 0;
}

int admin_student_search(const char *keyword, char *json_out, size_t size) {
    if (!keyword) {
        json_build_resp(json_out, size, 400, "缺少关键词参数", NULL);
        return -1;
    }

    Student list[MAX_STUDENTS];
    int count = student_search(keyword, list, MAX_STUDENTS);

    char data[65536];
    students_to_json(list, count, data, sizeof(data));
    json_build_resp(json_out, size, 200, "ok", data);
    return 0;
}

int admin_student_add(const char *json_body, char *json_out, size_t size) {
    if (!json_body) {
        json_build_resp(json_out, size, 400, "缺少请求数据", NULL);
        return -1;
    }

    Student s;
    memset(&s, 0, sizeof(s));

    char name[MAX_NAME_LEN], dept[MAX_DEPT_LEN], grade[16], phone[20], password[MAX_PWD_LEN];
    if (!json_get_string(json_body, "name", name, sizeof(name)) ||
        !json_get_string(json_body, "department", dept, sizeof(dept))) {
        json_build_resp(json_out, size, 400, "缺少必填字段: name, department", NULL);
        return -1;
    }

    /* 可选字段 */
    json_get_string(json_body, "grade", grade, sizeof(grade));
    json_get_string(json_body, "phone", phone, sizeof(phone));
    json_get_string(json_body, "password", password, sizeof(password));

    if (strlen(name) == 0 || strlen(dept) == 0) {
        json_build_resp(json_out, size, 400, "姓名和院系不能为空", NULL);
        return -1;
    }

    if (strlen(phone) > 0 && !validate_phone(phone)) {
        json_build_resp(json_out, size, 400, "手机号格式不正确", NULL);
        return -1;
    }

    /* 默认密码 */
    if (strlen(password) == 0) {
        strcpy(password, "123456");
    }

    /* 构建学生对象 */
    s.id = get_next_student_id();
    gen_student_no(s.id, s.student_no, sizeof(s.student_no));
    str_safe_copy(s.name, name, sizeof(s.name));
    str_safe_copy(s.department, dept, sizeof(s.department));
    str_safe_copy(s.grade, grade, sizeof(s.grade));
    str_safe_copy(s.phone, phone, sizeof(s.phone));
    password_hash(password, s.password, sizeof(s.password));

    int ret = student_add(&s);
    switch (ret) {
        case 0: {
            char data[1024];
            student_to_json(&s, data, sizeof(data));
            json_build_resp(json_out, size, 200, "添加成功", data);
            return 0;
        }
        case -2:
            json_build_resp(json_out, size, 409, "学号已存在", NULL);
            return -1;
        default:
            json_build_resp(json_out, size, 500, "添加失败", NULL);
            return -1;
    }
}

int admin_student_edit(const char *json_body, char *json_out, size_t size) {
    if (!json_body) {
        json_build_resp(json_out, size, 400, "缺少请求数据", NULL);
        return -1;
    }

    char student_no[32];
    if (!json_get_string(json_body, "student_no", student_no, sizeof(student_no))) {
        json_build_resp(json_out, size, 400, "缺少学号", NULL);
        return -1;
    }

    /* 查找现有学生 */
    Student s;
    if (student_find_by_no(student_no, &s) != 0) {
        json_build_resp(json_out, size, 404, "学生不存在", NULL);
        return -1;
    }

    /* 更新可修改字段 */
    char buf[MAX_FIELD_LEN];
    if (json_get_string(json_body, "name", buf, sizeof(buf)) && strlen(buf) > 0)
        str_safe_copy(s.name, buf, sizeof(s.name));
    if (json_get_string(json_body, "department", buf, sizeof(buf)) && strlen(buf) > 0)
        str_safe_copy(s.department, buf, sizeof(s.department));
    if (json_get_string(json_body, "grade", buf, sizeof(buf)) && strlen(buf) > 0)
        str_safe_copy(s.grade, buf, sizeof(s.grade));
    if (json_get_string(json_body, "phone", buf, sizeof(buf))) {
        if (strlen(buf) > 0 && !validate_phone(buf)) {
            json_build_resp(json_out, size, 400, "手机号格式不正确", NULL);
            return -1;
        }
        str_safe_copy(s.phone, buf, sizeof(s.phone));
    }

    int ret = student_update(&s);
    if (ret == 0) {
        json_build_resp(json_out, size, 200, "修改成功", NULL);
        return 0;
    } else {
        json_build_resp(json_out, size, 500, "修改失败", NULL);
        return -1;
    }
}

int admin_student_delete(const char *student_no, char *json_out, size_t size) {
    if (!student_no || strlen(student_no) == 0) {
        json_build_resp(json_out, size, 400, "缺少学号", NULL);
        return -1;
    }

    int ret = student_delete(student_no);
    switch (ret) {
        case 0:
            json_build_resp(json_out, size, 200, "删除成功", NULL);
            return 0;
        case -2:
            json_build_resp(json_out, size, 404, "学生不存在", NULL);
            return -1;
        default:
            json_build_resp(json_out, size, 500, "删除失败", NULL);
            return -1;
    }
}

/* ========== 校园卡管理 ========== */

int admin_card_list(char *json_out, size_t size) {
    Card list[MAX_CARDS];
    int count = card_find_all(list, MAX_CARDS);

    char data[131072];
    cards_to_json(list, count, data, sizeof(data));
    json_build_resp(json_out, size, 200, "ok", data);
    return 0;
}

int admin_card_search(const char *keyword, char *json_out, size_t size) {
    if (!keyword) {
        json_build_resp(json_out, size, 400, "缺少关键词参数", NULL);
        return -1;
    }

    Card list[MAX_CARDS];
    int count = card_search(keyword, list, MAX_CARDS);

    char data[131072];
    cards_to_json(list, count, data, sizeof(data));
    json_build_resp(json_out, size, 200, "ok", data);
    return 0;
}

int admin_card_issue(const char *json_body, char *json_out, size_t size) {
    if (!json_body) {
        json_build_resp(json_out, size, 400, "缺少请求数据", NULL);
        return -1;
    }

    char student_no[32];
    if (!json_get_string(json_body, "student_no", student_no, sizeof(student_no))) {
        json_build_resp(json_out, size, 400, "缺少学号", NULL);
        return -1;
    }

    /* 验证学生存在 */
    Student student;
    if (student_find_by_no(student_no, &student) != 0) {
        json_build_resp(json_out, size, 404, "学生不存在", NULL);
        return -1;
    }

    /* 构建卡对象 */
    Card c;
    memset(&c, 0, sizeof(c));
    c.id = get_next_card_id();
    gen_card_no(c.card_no, sizeof(c.card_no));
    str_safe_copy(c.student_no, student_no, sizeof(c.student_no));
    str_safe_copy(c.name, student.name, sizeof(c.name));
    c.balance = 0.0;
    c.status = CARD_ACTIVE;

    /* 默认每日限额 */
    double limit = 500.0;
    json_get_double(json_body, "daily_limit", &limit);
    c.daily_limit = (limit > 0 && limit <= 99999) ? limit : 500.0;
    date_now_str(c.issue_date, sizeof(c.issue_date));

    int ret = card_issue(&c);
    switch (ret) {
        case 0: {
            char data[1024];
            card_to_json(&c, data, sizeof(data));
            json_build_resp(json_out, size, 200, "办卡成功", data);
            return 0;
        }
        case -2:
            json_build_resp(json_out, size, 409, "卡号已存在", NULL);
            return -1;
        case -3:
            json_build_resp(json_out, size, 409, "该学生已有一张有效卡", NULL);
            return -1;
        default:
            json_build_resp(json_out, size, 500, "办卡失败", NULL);
            return -1;
    }
}

static int card_status_op(const char *card_no, char *json_out, size_t size,
                          int (*op)(const char *), const char *action_name) {
    if (!card_no || strlen(card_no) == 0) {
        json_build_resp(json_out, size, 400, "缺少卡号", NULL);
        return -1;
    }

    int ret = op(card_no);
    switch (ret) {
        case 0: {
            char msg[128];
            snprintf(msg, sizeof(msg), "%s成功", action_name);
            json_build_resp(json_out, size, 200, msg, NULL);
            return 0;
        }
        case -2:
            json_build_resp(json_out, size, 404, "卡不存在", NULL);
            return -1;
        case -4:
            json_build_resp(json_out, size, 400, "已注销的卡不能操作", NULL);
            return -1;
        case -5:
            json_build_resp(json_out, size, 400, "卡已是该状态", NULL);
            return -1;
        default:
            json_build_resp(json_out, size, 500, "操作失败", NULL);
            return -1;
    }
}

int admin_card_freeze(const char *card_no, char *json_out, size_t size) {
    return card_status_op(card_no, json_out, size, card_freeze, "冻结");
}

int admin_card_unfreeze(const char *card_no, char *json_out, size_t size) {
    return card_status_op(card_no, json_out, size, card_unfreeze, "解冻");
}

int admin_card_report_lost(const char *card_no, char *json_out, size_t size) {
    return card_status_op(card_no, json_out, size, card_report_lost, "挂失");
}

int admin_card_cancel(const char *card_no, char *json_out, size_t size) {
    return card_status_op(card_no, json_out, size, card_cancel, "注销");
}

int admin_card_recharge(const char *json_body, char *json_out, size_t size) {
    if (!json_body) {
        json_build_resp(json_out, size, 400, "缺少请求数据", NULL);
        return -1;
    }

    char card_no[32], desc[MAX_DESC_LEN];
    double amount;

    if (!json_get_string(json_body, "card_no", card_no, sizeof(card_no))) {
        json_build_resp(json_out, size, 400, "缺少卡号", NULL);
        return -1;
    }
    if (!json_get_double(json_body, "amount", &amount)) {
        json_build_resp(json_out, size, 400, "缺少充值金额", NULL);
        return -1;
    }
    json_get_string(json_body, "description", desc, sizeof(desc));
    if (strlen(desc) == 0) {
        strcpy(desc, "管理员充值");
    }

    int ret = card_recharge(card_no, amount, desc);
    switch (ret) {
        case 0: {
            Card c;
            card_find_by_no(card_no, &c);
            char data[1024];
            char card_json[512];
            card_to_json(&c, card_json, sizeof(card_json));
            snprintf(data, sizeof(data), "{\"card\":%s}", card_json);
            json_build_resp(json_out, size, 200, "充值成功", data);
            return 0;
        }
        case -2:
            json_build_resp(json_out, size, 404, "卡不存在", NULL);
            return -1;
        case -3:
            json_build_resp(json_out, size, 400, "卡状态异常，无法充值", NULL);
            return -1;
        default:
            json_build_resp(json_out, size, 500, "充值失败", NULL);
            return -1;
    }
}

/* ========== 交易记录 ========== */

int admin_record_list(char *json_out, size_t size) {
    Record list[MAX_RECORDS];
    int count = record_find_all(list, 500); /* 最多返回最近500条 */

    char data[262144];
    records_to_json(list, count, data, sizeof(data));
    json_build_resp(json_out, size, 200, "ok", data);
    return 0;
}

int admin_record_search(const char *keyword, char *json_out, size_t size) {
    if (!keyword) {
        json_build_resp(json_out, size, 400, "缺少关键词参数", NULL);
        return -1;
    }

    Record list[MAX_RECORDS];
    int count = record_search(keyword, list, 200);

    char data[131072];
    records_to_json(list, count, data, sizeof(data));
    json_build_resp(json_out, size, 200, "ok", data);
    return 0;
}

int admin_record_by_card(const char *card_no, char *json_out, size_t size) {
    if (!card_no || strlen(card_no) == 0) {
        json_build_resp(json_out, size, 400, "缺少卡号", NULL);
        return -1;
    }

    Record list[MAX_RECORDS];
    int count = record_find_by_card(card_no, list, 200);

    char data[131072];
    records_to_json(list, count, data, sizeof(data));
    json_build_resp(json_out, size, 200, "ok", data);
    return 0;
}

int admin_record_by_student(const char *student_no, char *json_out, size_t size) {
    if (!student_no || strlen(student_no) == 0) {
        json_build_resp(json_out, size, 400, "缺少学号", NULL);
        return -1;
    }

    Record list[MAX_RECORDS];
    int count = record_find_by_student(student_no, list, 200);

    char data[131072];
    records_to_json(list, count, data, sizeof(data));
    json_build_resp(json_out, size, 200, "ok", data);
    return 0;
}

/* ========== 系统统计 ========== */

int admin_stats(char *json_out, size_t size) {
    SystemStats stats;
    if (stats_get(&stats) != 0) {
        json_build_resp(json_out, size, 500, "获取统计失败", NULL);
        return -1;
    }

    char data[2048];
    snprintf(data, sizeof(data),
             "{\"total_students\":%d,\"total_cards\":%d,"
             "\"active_cards\":%d,\"frozen_cards\":%d,"
             "\"lost_cards\":%d,\"cancelled_cards\":%d,"
             "\"total_balance\":%.2f,"
             "\"today_recharges\":%d,\"today_recharge_amount\":%.2f,"
             "\"today_consumes\":%d,\"today_consume_amount\":%.2f}",
             stats.total_students, stats.total_cards,
             stats.active_cards, stats.frozen_cards,
             stats.lost_cards, stats.cancelled_cards,
             stats.total_balance,
             stats.today_recharges, stats.today_recharge_amount,
             stats.today_consumes, stats.today_consume_amount);
    json_build_resp(json_out, size, 200, "ok", data);
    return 0;
}
