/**
 * server.c — HTTP 服务器（基于 Mongoose v7 第三方库）
 *
 * 此文件提供:
 *   - 静态文件服务（web 目录下的 HTML/CSS/JS）
 *   - RESTful API 路由处理
 *   - 会话鉴权中间件
 *
 * 已适配 Mongoose v7.x API。
 */
#include "include/server.h"
#include "include/card_data.h"
#include "include/login.h"
#include "include/admin_op.h"
#include "include/student_op.h"
#include "include/utils.h"
#include "thirdparty/mongoose.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== 内部辅助 ========== */

#define RESP_BUF_SIZE   262144

/**
 * 发送 JSON 响应（Mongoose v7 使用 mg_http_reply）
 */
static void send_json(struct mg_connection *c, const char *json) {
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", json);
}

/**
 * 从 HTTP 请求中获取 Bearer Token（v7 使用 mg_http_get_header）
 */
static int get_bearer_token(struct mg_http_message *hm, char *token, size_t size) {
    struct mg_str *auth = mg_http_get_header(hm, "Authorization");
    if (!auth || auth->len < 7 || strncmp(auth->buf, "Bearer ", 7) != 0) {
        return -1;
    }
    str_safe_copy(token, auth->buf + 7, size);
    return 0;
}

/**
 * 鉴权检查 — 验证 token 并返回会话信息
 * 返回 0 表示鉴权通过
 */
static int check_auth(struct mg_connection *c, struct mg_http_message *hm, Session *session_out) {
    char token[65];
    if (get_bearer_token(hm, token, sizeof(token)) != 0) {
        char resp[256];
        json_build_resp(resp, sizeof(resp), 401, "未登录，请先登录", NULL);
        send_json(c, resp);
        return -1;
    }

    Session session;
    if (session_validate(token, &session) != 0) {
        char resp[256];
        json_build_resp(resp, sizeof(resp), 401, "登录已过期，请重新登录", NULL);
        send_json(c, resp);
        return -1;
    }

    if (session_out) {
        *session_out = session;
    }
    return 0;
}

/**
 * 读取请求 Body（v7 使用 hm->body）
 */
static int read_body(struct mg_http_message *hm, char *out, size_t size) {
    if (!hm->body.buf || hm->body.len <= 0) {
        out[0] = '\0';
        return -1;
    }

    size_t copy_len = (size_t)hm->body.len < size - 1 ? (size_t)hm->body.len : size - 1;
    memcpy(out, hm->body.buf, copy_len);
    out[copy_len] = '\0';
    return 0;
}

/**
 * 从 query string 获取变量（v7 使用 mg_http_var）
 * 返回指向变量值的指针，不存在时返回 NULL
 */
static const char *get_query_var(struct mg_http_message *hm, const char *name) {
    struct mg_str val = mg_http_var(hm->query, mg_str(name));
    return (val.buf != NULL && val.len > 0) ? val.buf : NULL;
}

/* ========== API 路由处理 ========== */

/**
 * POST /api/login
 * Body: { "account": "admin或学号", "password": "...", "role": 0或1 }
 */
static void handle_login(struct mg_connection *c, struct mg_http_message *hm) {
    char body[4096];
    if (read_body(hm, body, sizeof(body)) != 0) {
        char resp[256];
        json_build_resp(resp, sizeof(resp), 400, "缺少请求数据", NULL);
        send_json(c, resp);
        return;
    }

    char account[32], password[MAX_PWD_LEN];
    int role = -1;

    json_get_string(body, "account", account, sizeof(account));
    json_get_string(body, "password", password, sizeof(password));
    json_get_int(body, "role", &role);

    if (strlen(account) == 0 || strlen(password) == 0) {
        char resp[256];
        json_build_resp(resp, sizeof(resp), 400, "缺少账号或密码", NULL);
        send_json(c, resp);
        return;
    }

    char token[65];
    char resp[RESP_BUF_SIZE];

    if (role == ROLE_ADMIN) {
        /* 管理员登录 */
        int ret = admin_login(account, password);
        if (ret != 0) {
            json_build_resp(resp, sizeof(resp), 401, "管理员账号或密码错误", NULL);
            send_json(c, resp);
            return;
        }
        session_create(account, ROLE_ADMIN, "", token, sizeof(token));

        char data[256];
        snprintf(data, sizeof(data),
                 "{\"token\":\"%s\",\"role\":%d,\"account\":\"%s\"}",
                 token, ROLE_ADMIN, account);
        json_build_resp(resp, sizeof(resp), 200, "登录成功", data);
        send_json(c, resp);

    } else {
        /* 学生登录 */
        int ret = student_login(account, password);
        if (ret != 0) {
            const char *msg = (ret == -2) ? "学号不存在" : "密码错误";
            json_build_resp(resp, sizeof(resp), 401, msg, NULL);
            send_json(c, resp);
            return;
        }
        session_create(account, ROLE_STUDENT, account, token, sizeof(token));

        char data[256];
        snprintf(data, sizeof(data),
                 "{\"token\":\"%s\",\"role\":%d,\"student_no\":\"%s\"}",
                 token, ROLE_STUDENT, account);
        json_build_resp(resp, sizeof(resp), 200, "登录成功", data);
        send_json(c, resp);
    }
}

/**
 * POST /api/logout
 */
static void handle_logout(struct mg_connection *c, struct mg_http_message *hm) {
    char token[65];
    if (get_bearer_token(hm, token, sizeof(token)) == 0) {
        session_destroy(token);
    }

    char resp[256];
    json_build_resp(resp, sizeof(resp), 200, "已退出登录", NULL);
    send_json(c, resp);
}

/**
 * GET /api/students?keyword=xxx
 * 管理员查看学生列表/搜索
 */
static void handle_students_list(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;

    if (session.role != ROLE_ADMIN) {
        char resp[256];
        json_build_resp(resp, sizeof(resp), 403, "无权限", NULL);
        send_json(c, resp);
        return;
    }

    char resp[RESP_BUF_SIZE];
    const char *keyword = get_query_var(hm, "keyword");

    if (keyword && strlen(keyword) > 0) {
        admin_student_search(keyword, resp, sizeof(resp));
    } else {
        admin_student_list(resp, sizeof(resp));
    }
    send_json(c, resp);
}

/**
 * POST /api/student/add
 */
static void handle_student_add(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;

    if (session.role != ROLE_ADMIN) {
        char resp[256];
        json_build_resp(resp, sizeof(resp), 403, "无权限", NULL);
        send_json(c, resp);
        return;
    }

    char body[4096], resp[RESP_BUF_SIZE];
    if (read_body(hm, body, sizeof(body)) != 0) {
        json_build_resp(resp, sizeof(resp), 400, "缺少请求数据", NULL);
        send_json(c, resp);
        return;
    }

    admin_student_add(body, resp, sizeof(resp));
    send_json(c, resp);
}

/**
 * POST /api/student/edit
 */
static void handle_student_edit(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;

    if (session.role != ROLE_ADMIN) {
        char resp[256];
        json_build_resp(resp, sizeof(resp), 403, "无权限", NULL);
        send_json(c, resp);
        return;
    }

    char body[4096], resp[RESP_BUF_SIZE];
    if (read_body(hm, body, sizeof(body)) != 0) {
        json_build_resp(resp, sizeof(resp), 400, "缺少请求数据", NULL);
        send_json(c, resp);
        return;
    }

    admin_student_edit(body, resp, sizeof(resp));
    send_json(c, resp);
}

/**
 * POST /api/student/delete
 * Body: { "student_no": "STU20240001" }
 */
static void handle_student_delete(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;

    if (session.role != ROLE_ADMIN) {
        char resp[256];
        json_build_resp(resp, sizeof(resp), 403, "无权限", NULL);
        send_json(c, resp);
        return;
    }

    char body[1024], resp[RESP_BUF_SIZE];
    if (read_body(hm, body, sizeof(body)) != 0) {
        json_build_resp(resp, sizeof(resp), 400, "缺少请求数据", NULL);
        send_json(c, resp);
        return;
    }

    char student_no[32];
    json_get_string(body, "student_no", student_no, sizeof(student_no));
    admin_student_delete(student_no, resp, sizeof(resp));
    send_json(c, resp);
}

/**
 * GET /api/cards?keyword=xxx
 */
static void handle_cards_list(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;

    if (session.role != ROLE_ADMIN) {
        char resp[256];
        json_build_resp(resp, sizeof(resp), 403, "无权限", NULL);
        send_json(c, resp);
        return;
    }

    char resp[RESP_BUF_SIZE];
    const char *keyword = get_query_var(hm, "keyword");

    if (keyword && strlen(keyword) > 0) {
        admin_card_search(keyword, resp, sizeof(resp));
    } else {
        admin_card_list(resp, sizeof(resp));
    }
    send_json(c, resp);
}

/**
 * POST /api/card/issue
 */
static void handle_card_issue(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;

    if (session.role != ROLE_ADMIN) {
        char resp[256];
        json_build_resp(resp, sizeof(resp), 403, "无权限", NULL);
        send_json(c, resp);
        return;
    }

    char body[4096], resp[RESP_BUF_SIZE];
    if (read_body(hm, body, sizeof(body)) != 0) {
        json_build_resp(resp, sizeof(resp), 400, "缺少请求数据", NULL);
        send_json(c, resp);
        return;
    }

    admin_card_issue(body, resp, sizeof(resp));
    send_json(c, resp);
}

/**
 * POST /api/card/freeze
 * Body: { "card_no": "CARD..." }
 */
static void handle_card_freeze(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;
    if (session.role != ROLE_ADMIN) { char r[256]; json_build_resp(r,sizeof(r),403,"无权限",NULL); send_json(c,r); return; }

    char body[1024], resp[RESP_BUF_SIZE], card_no[32];
    read_body(hm, body, sizeof(body));
    json_get_string(body, "card_no", card_no, sizeof(card_no));
    admin_card_freeze(card_no, resp, sizeof(resp));
    send_json(c, resp);
}

/**
 * POST /api/card/unfreeze
 */
static void handle_card_unfreeze(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;
    if (session.role != ROLE_ADMIN) { char r[256]; json_build_resp(r,sizeof(r),403,"无权限",NULL); send_json(c,r); return; }

    char body[1024], resp[RESP_BUF_SIZE], card_no[32];
    read_body(hm, body, sizeof(body));
    json_get_string(body, "card_no", card_no, sizeof(card_no));
    admin_card_unfreeze(card_no, resp, sizeof(resp));
    send_json(c, resp);
}

/**
 * POST /api/card/report-lost
 */
static void handle_card_report_lost(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;
    if (session.role != ROLE_ADMIN) { char r[256]; json_build_resp(r,sizeof(r),403,"无权限",NULL); send_json(c,r); return; }

    char body[1024], resp[RESP_BUF_SIZE], card_no[32];
    read_body(hm, body, sizeof(body));
    json_get_string(body, "card_no", card_no, sizeof(card_no));
    admin_card_report_lost(card_no, resp, sizeof(resp));
    send_json(c, resp);
}

/**
 * POST /api/card/cancel
 */
static void handle_card_cancel(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;
    if (session.role != ROLE_ADMIN) { char r[256]; json_build_resp(r,sizeof(r),403,"无权限",NULL); send_json(c,r); return; }

    char body[1024], resp[RESP_BUF_SIZE], card_no[32];
    read_body(hm, body, sizeof(body));
    json_get_string(body, "card_no", card_no, sizeof(card_no));
    admin_card_cancel(card_no, resp, sizeof(resp));
    send_json(c, resp);
}

/**
 * POST /api/card/recharge
 */
static void handle_card_recharge(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;
    if (session.role != ROLE_ADMIN) { char r[256]; json_build_resp(r,sizeof(r),403,"无权限",NULL); send_json(c,r); return; }

    char body[4096], resp[RESP_BUF_SIZE];
    if (read_body(hm, body, sizeof(body)) != 0) {
        json_build_resp(resp, sizeof(resp), 400, "缺少请求数据", NULL);
        send_json(c, resp);
        return;
    }
    admin_card_recharge(body, resp, sizeof(resp));
    send_json(c, resp);
}

/**
 * POST /api/card/consume
 * 消费接口（学生端或刷卡终端调用）
 */
static void handle_card_consume(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;

    char body[4096], resp[RESP_BUF_SIZE];
    if (read_body(hm, body, sizeof(body)) != 0) {
        json_build_resp(resp, sizeof(resp), 400, "缺少请求数据", NULL);
        send_json(c, resp);
        return;
    }
    student_consume(body, resp, sizeof(resp));
    send_json(c, resp);
}

/**
 * GET /api/records?card_no=xxx&student_no=xxx&keyword=xxx
 */
static void handle_records(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;

    if (session.role != ROLE_ADMIN) {
        char resp[256];
        json_build_resp(resp, sizeof(resp), 403, "无权限", NULL);
        send_json(c, resp);
        return;
    }

    char resp[RESP_BUF_SIZE];
    const char *card_no = get_query_var(hm, "card_no");
    const char *student_no = get_query_var(hm, "student_no");
    const char *keyword = get_query_var(hm, "keyword");

    if (card_no && strlen(card_no) > 0) {
        admin_record_by_card(card_no, resp, sizeof(resp));
    } else if (student_no && strlen(student_no) > 0) {
        admin_record_by_student(student_no, resp, sizeof(resp));
    } else if (keyword && strlen(keyword) > 0) {
        admin_record_search(keyword, resp, sizeof(resp));
    } else {
        admin_record_list(resp, sizeof(resp));
    }
    send_json(c, resp);
}

/**
 * GET /api/my-card
 */
static void handle_my_card(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;

    char resp[RESP_BUF_SIZE];
    student_my_card(session.student_no, resp, sizeof(resp));
    send_json(c, resp);
}

/**
 * GET /api/my-records
 */
static void handle_my_records(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;

    char resp[RESP_BUF_SIZE];
    student_my_records(session.student_no, resp, sizeof(resp));
    send_json(c, resp);
}

/**
 * POST /api/change-pwd
 */
static void handle_change_pwd(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;

    char body[1024], resp[RESP_BUF_SIZE];
    if (read_body(hm, body, sizeof(body)) != 0) {
        json_build_resp(resp, sizeof(resp), 400, "缺少请求数据", NULL);
        send_json(c, resp);
        return;
    }

    student_change_pwd(session.student_no, body, resp, sizeof(resp));
    send_json(c, resp);
}

/**
 * GET /api/my-info
 */
static void handle_my_info(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;

    char resp[RESP_BUF_SIZE];
    student_my_info(session.student_no, resp, sizeof(resp));
    send_json(c, resp);
}

/**
 * GET /api/stats
 */
static void handle_stats(struct mg_connection *c, struct mg_http_message *hm) {
    Session session;
    if (check_auth(c, hm, &session) != 0) return;

    if (session.role != ROLE_ADMIN) {
        char resp[256];
        json_build_resp(resp, sizeof(resp), 403, "无权限", NULL);
        send_json(c, resp);
        return;
    }

    char resp[RESP_BUF_SIZE];
    admin_stats(resp, sizeof(resp));
    send_json(c, resp);
}

/* ========== 请求分发 ========== */

/**
 * Mongoose v7 事件处理回调（无 fn_data 参数）
 * 所有 HTTP 请求在此分发到对应的处理器
 */
static void event_handler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;

        /* ===== API 路由表 ===== */

        if (mg_match(hm->uri, mg_str("/api/login"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
            handle_login(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/logout"), NULL)) {
            handle_logout(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/students"), NULL) && mg_match(hm->method, mg_str("GET"), NULL)) {
            handle_students_list(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/student/add"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
            handle_student_add(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/student/edit"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
            handle_student_edit(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/student/delete"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
            handle_student_delete(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/cards"), NULL) && mg_match(hm->method, mg_str("GET"), NULL)) {
            handle_cards_list(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/card/issue"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
            handle_card_issue(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/card/freeze"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
            handle_card_freeze(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/card/unfreeze"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
            handle_card_unfreeze(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/card/report-lost"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
            handle_card_report_lost(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/card/cancel"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
            handle_card_cancel(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/card/recharge"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
            handle_card_recharge(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/card/consume"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
            handle_card_consume(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/records"), NULL) && mg_match(hm->method, mg_str("GET"), NULL)) {
            handle_records(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/my-card"), NULL) && mg_match(hm->method, mg_str("GET"), NULL)) {
            handle_my_card(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/my-records"), NULL) && mg_match(hm->method, mg_str("GET"), NULL)) {
            handle_my_records(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/change-pwd"), NULL) && mg_match(hm->method, mg_str("POST"), NULL)) {
            handle_change_pwd(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/my-info"), NULL) && mg_match(hm->method, mg_str("GET"), NULL)) {
            handle_my_info(c, hm);
            return;
        }
        if (mg_match(hm->uri, mg_str("/api/stats"), NULL) && mg_match(hm->method, mg_str("GET"), NULL)) {
            handle_stats(c, hm);
            return;
        }

        /* -- 未匹配 API 路由，尝试静态文件 -- */
        struct mg_http_serve_opts opts = { .root_dir = WEB_ROOT };
        mg_http_serve_dir(c, hm, &opts);
    }
}

/* ========== 公共接口 ========== */

/* mongoose.h 已声明 extern struct mg_mgr g_mgr，此处定义 */
struct mg_mgr g_mgr;
static int g_running = 0;

int server_start(const char *port) {
    mg_mgr_init(&g_mgr);

    const char *listen_port = port ? port : SERVER_PORT;
    char listen_addr[64];
    snprintf(listen_addr, sizeof(listen_addr), "http://0.0.0.0:%s", listen_port);

    if (mg_http_listen(&g_mgr, listen_addr, event_handler, NULL) == NULL) {
        fprintf(stderr, "[ERROR] 无法监听端口 %s\n", listen_port);
        mg_mgr_free(&g_mgr);
        return -1;
    }

    g_running = 1;
    printf("[SERVER] 校园卡系统服务器启动成功\n");
    printf("[SERVER] 监听地址: %s\n", listen_addr);
    printf("[SERVER] 管理后台: http://localhost:%s/admin.html\n", listen_port);
    printf("[SERVER] 学生端:   http://localhost:%s/student.html\n", listen_port);
    printf("[SERVER] 按 Ctrl+C 停止服务器\n");

    /* 事件循环 */
    while (g_running) {
        mg_mgr_poll(&g_mgr, 1000);
    }

    mg_mgr_free(&g_mgr);
    return 0;
}

void server_stop(void) {
    g_running = 0;
}
