#ifndef ADMIN_OP_H
#define ADMIN_OP_H

#include "card_data.h"
#include "login.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========== 管理员操作 API ========== */

/* 学生管理 */
int  admin_student_list(char *json_out, size_t size);
int  admin_student_search(const char *keyword, char *json_out, size_t size);
int  admin_student_add(const char *json_body, char *json_out, size_t size);
int  admin_student_edit(const char *json_body, char *json_out, size_t size);
int  admin_student_delete(const char *student_no, char *json_out, size_t size);

/* 校园卡管理 */
int  admin_card_list(char *json_out, size_t size);
int  admin_card_search(const char *keyword, char *json_out, size_t size);
int  admin_card_issue(const char *json_body, char *json_out, size_t size);
int  admin_card_freeze(const char *card_no, char *json_out, size_t size);
int  admin_card_unfreeze(const char *card_no, char *json_out, size_t size);
int  admin_card_report_lost(const char *card_no, char *json_out, size_t size);
int  admin_card_cancel(const char *card_no, char *json_out, size_t size);
int  admin_card_recharge(const char *json_body, char *json_out, size_t size);

/* 交易记录 */
int  admin_record_list(char *json_out, size_t size);
int  admin_record_search(const char *keyword, char *json_out, size_t size);
int  admin_record_by_card(const char *card_no, char *json_out, size_t size);
int  admin_record_by_student(const char *student_no, char *json_out, size_t size);

/* 系统统计 */
int  admin_stats(char *json_out, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* ADMIN_OP_H */
