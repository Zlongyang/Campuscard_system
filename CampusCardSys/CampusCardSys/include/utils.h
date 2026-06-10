#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========== 字符串工具 ========== */

/* 去除字符串首尾空白 */
char *str_trim(char *str);

/* 字符串转小写 */
char *str_tolower(char *str);

/* 安全的字符串复制 */
char *str_safe_copy(char *dst, const char *src, size_t dst_size);

/* ========== 哈希 / 加密 ========== */

/* 简单密码哈希 (SHA-256 简化版，实际项目请用 openssl) */
void password_hash(const char *password, char *out, size_t out_size);

/* 验证密码 */
int  password_verify(const char *password, const char *hash);

/* ========== 时间工具 ========== */

/* 获取当前时间字符串 "YYYY-MM-DD HH:MM:SS" */
void time_now_str(char *out, size_t out_size);

/* 获取当前日期字符串 "YYYY-MM-DD" */
void date_now_str(char *out, size_t out_size);

/* ========== ID 生成 ========== */

/* 生成卡号: CARD + 时间戳 + 随机数 */
void gen_card_no(char *out, size_t out_size);

/* 生成学号: STU + 年份 + 序号 */
void gen_student_no(int seq, char *out, size_t out_size);

/* ========== 验证工具 ========== */

/* 验证学号格式 */
int  validate_student_no(const char *no);

/* 验证卡号格式 */
int  validate_card_no(const char *no);

/* 验证手机号格式 */
int  validate_phone(const char *phone);

/* 验证金额是否合法 */
int  validate_amount(double amount);

/* ========== JSON 工具（简易版，避免引入第三方库） ========== */

/* 构建简单 JSON 响应字符串 */
void json_build_resp(char *out, size_t out_size, int code, const char *msg, const char *data);

/* 从 JSON 字符串中提取指定 key 的值（简易版） */
int  json_get_string(const char *json, const char *key, char *out, size_t out_size);
int  json_get_double(const char *json, const char *key, double *out);
int  json_get_int(const char *json, const char *key, int *out);

/* URL 解码 */
void url_decode(char *dst, const char *src);

#ifdef __cplusplus
}
#endif

#endif /* UTILS_H */
