/**
 * student_op.c — 学生操作 API 实现
 * 查询卡信息、交易记录、消费、修改密码等
 */
#include "include/student_op.h"
#include "include/file_io.h"
#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== 内部辅助 ========== */

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

/* ========== 查询我的校园卡 ========== */

int student_my_card(const char *student_no, char *json_out, size_t size) {
    if (!student_no) {
        json_build_resp(json_out, size, 400, "参数错误", NULL);
        return -1;
    }

    Card c;
    if (card_find_by_student(student_no, &c) != 0) {
        json_build_resp(json_out, size, 404, "未找到校园卡，请先办卡", NULL);
        return -1;
    }

    char data[1024];
    card_to_json(&c, data, sizeof(data));
    json_build_resp(json_out, size, 200, "ok", data);
    return 0;
}

/* ========== 查询我的交易记录 ========== */

int student_my_records(const char *student_no, char *json_out, size_t size) {
    if (!student_no) {
        json_build_resp(json_out, size, 400, "参数错误", NULL);
        return -1;
    }

    Record list[MAX_RECORDS];
    int count = record_find_by_student(student_no, list, 200);

    char data[131072];
    records_to_json(list, count, data, sizeof(data));
    json_build_resp(json_out, size, 200, "ok", data);
    return 0;
}

/* ========== 消费 ========== */

int student_consume(const char *json_body, char *json_out, size_t size) {
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
        json_build_resp(json_out, size, 400, "缺少消费金额", NULL);
        return -1;
    }
    json_get_string(json_body, "description", desc, sizeof(desc));
    if (strlen(desc) == 0) {
        strcpy(desc, "食堂消费");
    }

    int ret = card_consume(card_no, amount, desc);
    switch (ret) {
        case 0: {
            Card c;
            card_find_by_no(card_no, &c);
            char card_json[512], data[1024];
            card_to_json(&c, card_json, sizeof(card_json));
            snprintf(data, sizeof(data),
                     "{\"amount\":%.2f,\"balance\":%.2f,\"card\":%s}",
                     amount, c.balance, card_json);
            json_build_resp(json_out, size, 200, "消费成功", data);
            return 0;
        }
        case -2:
            json_build_resp(json_out, size, 404, "卡不存在", NULL);
            return -1;
        case -3:
            json_build_resp(json_out, size, 400, "卡状态异常，无法消费", NULL);
            return -1;
        case -4:
            json_build_resp(json_out, size, 400, "余额不足", NULL);
            return -1;
        case -5:
            json_build_resp(json_out, size, 400, "超过每日消费限额", NULL);
            return -1;
        default:
            json_build_resp(json_out, size, 500, "消费失败", NULL);
            return -1;
    }
}

/* ========== 修改密码 ========== */

int student_change_pwd(const char *student_no, const char *json_body, char *json_out, size_t size) {
    if (!student_no || !json_body) {
        json_build_resp(json_out, size, 400, "参数错误", NULL);
        return -1;
    }

    char old_pwd[MAX_PWD_LEN], new_pwd[MAX_PWD_LEN];
    if (!json_get_string(json_body, "old_password", old_pwd, sizeof(old_pwd)) ||
        !json_get_string(json_body, "new_password", new_pwd, sizeof(new_pwd))) {
        json_build_resp(json_out, size, 400, "缺少旧密码或新密码", NULL);
        return -1;
    }

    int ret = student_change_password(student_no, old_pwd, new_pwd);
    switch (ret) {
        case 0:
            json_build_resp(json_out, size, 200, "密码修改成功", NULL);
            return 0;
        case -2:
            json_build_resp(json_out, size, 404, "学生不存在", NULL);
            return -1;
        case -3:
            json_build_resp(json_out, size, 400, "旧密码错误", NULL);
            return -1;
        case -4:
            json_build_resp(json_out, size, 400, "新密码太短（至少6位）", NULL);
            return -1;
        default:
            json_build_resp(json_out, size, 500, "修改失败", NULL);
            return -1;
    }
}

/* ========== 查询个人信息 ========== */

int student_my_info(const char *student_no, char *json_out, size_t size) {
    if (!student_no) {
        json_build_resp(json_out, size, 400, "参数错误", NULL);
        return -1;
    }

    Student s;
    if (student_find_by_no(student_no, &s) != 0) {
        json_build_resp(json_out, size, 404, "学生不存在", NULL);
        return -1;
    }

    char data[1024];
    snprintf(data, sizeof(data),
             "{\"id\":%d,\"student_no\":\"%s\",\"name\":\"%s\","
             "\"department\":\"%s\",\"grade\":\"%s\",\"phone\":\"%s\"}",
             s.id, s.student_no, s.name, s.department, s.grade, s.phone);
    json_build_resp(json_out, size, 200, "ok", data);
    return 0;
}
