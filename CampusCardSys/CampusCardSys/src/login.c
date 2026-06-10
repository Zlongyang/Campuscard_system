/**
 * login.c — 登录认证与会话管理
 * 支持管理员登录和学生登录
 */
#include "include/login.h"
#include "include/file_io.h"
#include "include/utils.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ========== 会话存储 ========== */
static Session g_sessions[MAX_SESSIONS];
static int g_session_count = 0;

/* ========== 初始化 ========== */

int login_init(void) {
    file_ensure_all();
    g_session_count = 0;
    memset(g_sessions, 0, sizeof(g_sessions));

    /* 初始化随机种子 */
    srand((unsigned int)time(NULL));

    return 0;
}

/* ========== 管理员登录 ========== */

int admin_login(const char *username, const char *password) {
    if (!username || !password) return -1;

    /* 验证管理员账号（硬编码，可从配置文件读取） */
    if (strcmp(username, ADMIN_USERNAME) != 0) {
        return -2; /* 管理员不存在 */
    }

    if (strcmp(password, ADMIN_PASSWORD) != 0) {
        return -3; /* 密码错误 */
    }

    return 0;
}

/* ========== 学生登录 ========== */

int student_login(const char *student_no, const char *password) {
    if (!student_no || !password) return -1;

    Student student;
    if (student_find_by_no(student_no, &student) != 0) {
        return -2; /* 学号不存在 */
    }

    if (!password_verify(password, student.password)) {
        return -3; /* 密码错误 */
    }

    return 0;
}

/* ========== 会话管理 ========== */

/* 生成随机 token */
static void gen_token(char *out, size_t size) {
    static const char *chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t len = size - 1;
    for (size_t i = 0; i < len; i++) {
        out[i] = chars[rand() % 62];
    }
    out[len] = '\0';
}

int session_create(const char *account, int role, const char *student_no, char *token_out, size_t size) {
    if (!account || !token_out || size < 17) return -1;

    /* 清理过期会话 */
    session_cleanup_expired();

    /* 检查是否超出最大会话数 */
    if (g_session_count >= MAX_SESSIONS) {
        session_cleanup_expired();
        if (g_session_count >= MAX_SESSIONS) {
            return -2; /* 会话已满 */
        }
    }

    /* 创建新会话 */
    Session *s = &g_sessions[g_session_count];
    s->id = g_session_count + 1;
    gen_token(s->token, sizeof(s->token));
    str_safe_copy(s->account, account, sizeof(s->account));
    s->role = role;
    if (student_no) {
        str_safe_copy(s->student_no, student_no, sizeof(s->student_no));
    }
    s->login_time = time(NULL);
    s->last_active = s->login_time;

    str_safe_copy(token_out, s->token, size);
    g_session_count++;

    return 0;
}

int session_validate(const char *token, Session *out) {
    if (!token) return -1;

    time_t now = time(NULL);

    for (int i = 0; i < g_session_count; i++) {
        if (strcmp(g_sessions[i].token, token) == 0) {
            /* 检查是否过期 */
            if (now - g_sessions[i].last_active > SESSION_EXPIRE) {
                /* 移除过期会话 */
                for (int j = i; j < g_session_count - 1; j++) {
                    g_sessions[j] = g_sessions[j + 1];
                }
                g_session_count--;
                return -3; /* 会话已过期 */
            }

            /* 更新活跃时间 */
            g_sessions[i].last_active = now;

            if (out) {
                *out = g_sessions[i];
            }
            return 0;
        }
    }

    return -2; /* 无效 token */
}

int session_destroy(const char *token) {
    if (!token) return -1;

    for (int i = 0; i < g_session_count; i++) {
        if (strcmp(g_sessions[i].token, token) == 0) {
            /* 移除会话 */
            for (int j = i; j < g_session_count - 1; j++) {
                g_sessions[j] = g_sessions[j + 1];
            }
            g_session_count--;
            return 0;
        }
    }

    return -2;
}

void session_cleanup_expired(void) {
    time_t now = time(NULL);
    int write_idx = 0;

    for (int i = 0; i < g_session_count; i++) {
        if (now - g_sessions[i].last_active <= SESSION_EXPIRE) {
            if (write_idx != i) {
                g_sessions[write_idx] = g_sessions[i];
            }
            write_idx++;
        }
    }
    g_session_count = write_idx;
}

/* ========== 密码修改 ========== */

int student_change_password(const char *student_no, const char *old_pwd, const char *new_pwd) {
    if (!student_no || !old_pwd || !new_pwd) return -1;

    /* 验证旧密码 */
    Student student;
    if (student_find_by_no(student_no, &student) != 0) {
        return -2;
    }

    if (!password_verify(old_pwd, student.password)) {
        return -3; /* 旧密码错误 */
    }

    /* 更新密码 */
    if (strlen(new_pwd) < 6) {
        return -4; /* 新密码太短 */
    }

    char new_hash[17];
    password_hash(new_pwd, new_hash, sizeof(new_hash));
    str_safe_copy(student.password, new_hash, sizeof(student.password));

    return student_update(&student);
}
