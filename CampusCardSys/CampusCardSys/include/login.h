#ifndef LOGIN_H
#define LOGIN_H

#include "card_data.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========== 登录会话 ========== */

#define MAX_SESSIONS    256
#define SESSION_EXPIRE  3600   /* 会话过期时间(秒) */

typedef struct {
    int     id;
    char    token[65];         /* 会话令牌 */
    char    account[32];       /* 登录账号（学号或admin） */
    int     role;              /* 0=管理员 1=学生 */
    char    student_no[32];    /* 如果是学生，记录学号 */
    time_t  login_time;
    time_t  last_active;
} Session;

/* 管理员账号 */
#define ADMIN_USERNAME  "admin"
#define ADMIN_PASSWORD  "admin123"   /* 默认密码，生产环境需修改 */

/* ========== 函数声明 ========== */

/* 初始化登录系统 */
int  login_init(void);

/* 管理员登录 */
int  admin_login(const char *username, const char *password);

/* 学生登录 */
int  student_login(const char *student_no, const char *password);

/* 会话管理 */
int  session_create(const char *account, int role, const char *student_no, char *token_out, size_t size);
int  session_validate(const char *token, Session *out);
int  session_destroy(const char *token);
void session_cleanup_expired(void);

/* 学生修改密码 */
int  student_change_password(const char *student_no, const char *old_pwd, const char *new_pwd);

#ifdef __cplusplus
}
#endif

#endif /* LOGIN_H */
