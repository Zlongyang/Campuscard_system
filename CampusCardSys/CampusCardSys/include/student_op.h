#ifndef STUDENT_OP_H
#define STUDENT_OP_H

#include "card_data.h"
#include "login.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========== 学生操作 API ========== */

/* 查询我的校园卡 */
int  student_my_card(const char *student_no, char *json_out, size_t size);

/* 查询我的交易记录 */
int  student_my_records(const char *student_no, char *json_out, size_t size);

/* 消费（食堂、超市等终端调用） */
int  student_consume(const char *json_body, char *json_out, size_t size);

/* 修改密码 */
int  student_change_pwd(const char *student_no, const char *json_body, char *json_out, size_t size);

/* 查询个人信息 */
int  student_my_info(const char *student_no, char *json_out, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* STUDENT_OP_H */
