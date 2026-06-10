/**
 * utils.c — 工具函数实现
 * 包含: 字符串处理、密码哈希、时间、JSON简易解析、校验
 */
#include "include/utils.h"
#include <ctype.h>
#include <stdarg.h>

/* ========== 字符串工具 ========== */

char *str_trim(char *str) {
    if (!str || !*str) return str;

    /* 去除尾部空白 */
    char *end = str + strlen(str) - 1;
    while (end >= str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }

    /* 去除首部空白 */
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    return str;
}

char *str_tolower(char *str) {
    if (!str) return str;
    for (char *p = str; *p; p++) {
        *p = (char)tolower((unsigned char)*p);
    }
    return str;
}

char *str_safe_copy(char *dst, const char *src, size_t dst_size) {
    if (!dst || dst_size == 0) return dst;
    if (!src) {
        dst[0] = '\0';
        return dst;
    }
    size_t i = 0;
    while (i < dst_size - 1 && src[i]) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
    return dst;
}

/* ========== 密码哈希 ========== */

/* 简化的哈希实现 — 实际项目建议使用 bcrypt/scrypt/PBKDF2 */
void password_hash(const char *password, char *out, size_t out_size) {
    if (!password || !out || out_size < 17) {
        if (out && out_size > 0) out[0] = '\0';
        return;
    }

    /* 简单 XOR + 位移哈希，输出16字符十六进制 */
    unsigned long hash = 5381;
    const char *salt = "@CampusCard#2024!";
    size_t salt_len = strlen(salt);
    size_t pw_len = strlen(password);

    for (size_t i = 0; i < pw_len; i++) {
        hash = ((hash << 5) + hash) + (unsigned char)password[i];
        hash ^= (unsigned char)salt[i % salt_len];
    }

    /* 多轮混淆 */
    for (int round = 0; round < 1000; round++) {
        hash = ((hash << 7) + hash) ^ 0xA5A5A5A5;
        hash = (hash >> 3) | (hash << 29);
        hash += (unsigned char)password[round % pw_len];
        hash ^= (unsigned char)salt[round % salt_len];
    }

    snprintf(out, out_size, "%016lx", hash);
}

int password_verify(const char *password, const char *hash) {
    if (!password || !hash) return 0;
    char computed[17];
    password_hash(password, computed, sizeof(computed));
    return (strcmp(computed, hash) == 0);
}

/* ========== 时间工具 ========== */

void time_now_str(char *out, size_t out_size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(out, out_size, "%Y-%m-%d %H:%M:%S", t);
}

void date_now_str(char *out, size_t out_size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(out, out_size, "%Y-%m-%d", t);
}

/* ========== ID 生成 ========== */

void gen_card_no(char *out, size_t out_size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    int rand_num = rand() % 10000;
    snprintf(out, out_size, "CARD%04d%02d%02d%04d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, rand_num);
}

void gen_student_no(int seq, char *out, size_t out_size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(out, out_size, "STU%04d%04d", t->tm_year + 1900, seq);
}

/* ========== 验证工具 ========== */

int validate_student_no(const char *no) {
    if (!no || strlen(no) < 4) return 0;
    /* 必须以字母开头，只含字母数字 */
    if (!isalpha((unsigned char)no[0])) return 0;
    for (const char *p = no; *p; p++) {
        if (!isalnum((unsigned char)*p)) return 0;
    }
    return 1;
}

int validate_card_no(const char *no) {
    if (!no || strlen(no) < 4) return 0;
    if (strncmp(no, "CARD", 4) != 0) return 0;
    return 1;
}

int validate_phone(const char *phone) {
    if (!phone) return 0;
    size_t len = strlen(phone);
    if (len != 11) return 0;
    if (phone[0] != '1') return 0;
    for (size_t i = 0; i < len; i++) {
        if (!isdigit((unsigned char)phone[i])) return 0;
    }
    return 1;
}

int validate_amount(double amount) {
    return (amount > 0 && amount <= 999999.99);
}

/* ========== JSON 工具（简易版） ========== */

void json_build_resp(char *out, size_t out_size, int code, const char *msg, const char *data) {
    if (!out || out_size == 0) return;

    if (data && data[0]) {
        snprintf(out, out_size,
                 "{\"code\":%d,\"msg\":\"%s\",\"data\":%s}",
                 code, msg ? msg : "", data);
    } else {
        snprintf(out, out_size,
                 "{\"code\":%d,\"msg\":\"%s\",\"data\":null}",
                 code, msg ? msg : "");
    }
}

/* 从 JSON 中提取字符串值（超简易解析器） */
int json_get_string(const char *json, const char *key, char *out, size_t out_size) {
    if (!json || !key || !out || out_size == 0) return 0;

    /* 构造搜索模式 "key" */
    char pattern[128];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);

    const char *pos = strstr(json, pattern);
    if (!pos) return 0;

    /* 跳过 key 和中间的 ":"  */
    pos += strlen(pattern);
    while (*pos && (*pos == ':' || *pos == ' ' || *pos == '\t')) pos++;

    if (*pos != '"') return 0;
    pos++; /* 跳过第一个引号 */

    /* 复制值直到遇到结束引号 */
    size_t i = 0;
    while (*pos && *pos != '"' && i < out_size - 1) {
        /* 处理转义字符 */
        if (*pos == '\\' && *(pos + 1)) {
            pos++;
            switch (*pos) {
                case '"':  out[i++] = '"';  break;
                case '\\': out[i++] = '\\'; break;
                case '/':  out[i++] = '/';  break;
                case 'n':  out[i++] = '\n'; break;
                case 't':  out[i++] = '\t'; break;
                default:   out[i++] = *pos; break;
            }
        } else {
            out[i++] = *pos;
        }
        pos++;
    }
    out[i] = '\0';
    return 1;
}

int json_get_double(const char *json, const char *key, double *out) {
    char buf[64];
    if (!json_get_string(json, key, buf, sizeof(buf))) return 0;
    char *endptr = NULL;
    *out = strtod(buf, &endptr);
    return (endptr != buf);
}

int json_get_int(const char *json, const char *key, int *out) {
    char buf[64];
    if (!json_get_string(json, key, buf, sizeof(buf))) return 0;
    char *endptr = NULL;
    long val = strtol(buf, &endptr, 10);
    *out = (int)val;
    return (endptr != buf);
}

/* URL 解码 */
void url_decode(char *dst, const char *src) {
    if (!dst || !src) return;
    char *d = dst;
    while (*src) {
        if (*src == '%' && isxdigit((unsigned char)*(src + 1)) && isxdigit((unsigned char)*(src + 2))) {
            unsigned int hi = (unsigned char)*(src + 1);
            unsigned int lo = (unsigned char)*(src + 2);
            hi = (hi > '9') ? ((hi & 0xDF) - 'A' + 10) : (hi - '0');
            lo = (lo > '9') ? ((lo & 0xDF) - 'A' + 10) : (lo - '0');
            *d++ = (char)((hi << 4) | lo);
            src += 3;
        } else if (*src == '+') {
            *d++ = ' ';
            src++;
        } else {
            *d++ = *src++;
        }
    }
    *d = '\0';
}
