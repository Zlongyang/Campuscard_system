#ifndef CARD_DATA_H
#define CARD_DATA_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========== 常量定义 ========== */
#define MAX_STUDENTS      10000
#define MAX_CARDS         10000
#define MAX_RECORDS       100000
#define MAX_LINE_LEN      512
#define MAX_FIELD_LEN     128
#define MAX_NAME_LEN      64
#define MAX_PWD_LEN       64
#define MAX_DEPT_LEN      64
#define MAX_DESC_LEN      256

/* 卡状态 */
#define CARD_ACTIVE       0   /* 正常 */
#define CARD_FROZEN       1   /* 冻结 */
#define CARD_LOST         2   /* 挂失 */
#define CARD_CANCELLED    3   /* 注销 */

/* 交易类型 */
#define REC_RECHARGE      0   /* 充值 */
#define REC_CONSUME       1   /* 消费 */
#define REC_REFUND        2   /* 退款 */

/* 角色 */
#define ROLE_ADMIN        0
#define ROLE_STUDENT      1

/* ========== 数据结构 ========== */

/* 学生信息 */
typedef struct {
    int     id;
    char    student_no[32];   /* 学号，唯一 */
    char    name[MAX_NAME_LEN];
    char    department[MAX_DEPT_LEN];
    char    grade[16];
    char    phone[20];
    char    password[MAX_PWD_LEN];
} Student;

/* 校园卡信息 */
typedef struct {
    int     id;
    char    card_no[32];      /* 卡号，唯一 */
    char    student_no[32];   /* 所属学生学号 */
    char    name[MAX_NAME_LEN];
    double  balance;          /* 余额 */
    int     status;           /* 0=正常 1=冻结 2=挂失 3=注销 */
    double  daily_limit;      /* 每日消费限额 */
    char    issue_date[20];
} Card;

/* 交易记录 */
typedef struct {
    int     id;
    char    card_no[32];
    char    student_no[32];
    int     type;             /* 0=充值 1=消费 2=退款 */
    double  amount;
    double  balance_before;
    double  balance_after;
    char    time_str[32];
    char    description[MAX_DESC_LEN];
} Record;

/* 统计数据 */
typedef struct {
    int     total_students;
    int     total_cards;
    int     active_cards;
    int     frozen_cards;
    int     lost_cards;
    int     cancelled_cards;
    double  total_balance;
    int     today_recharges;
    double  today_recharge_amount;
    int     today_consumes;
    double  today_consume_amount;
} SystemStats;

/* ========== 卡操作函数声明 ========== */

/* 学生管理 */
int  student_add(const Student *s);
int  student_update(const Student *s);
int  student_delete(const char *student_no);
int  student_find_by_no(const char *student_no, Student *out);
int  student_find_all(Student *out, int max_count);
int  student_search(const char *keyword, Student *out, int max_count);

/* 校园卡管理 */
int  card_issue(const Card *c);
int  card_freeze(const char *card_no);
int  card_unfreeze(const char *card_no);
int  card_report_lost(const char *card_no);
int  card_cancel(const char *card_no);
int  card_recharge(const char *card_no, double amount, const char *desc);
int  card_consume(const char *card_no, double amount, const char *desc);
int  card_find_by_no(const char *card_no, Card *out);
int  card_find_by_student(const char *student_no, Card *out);
int  card_find_all(Card *out, int max_count);
int  card_search(const char *keyword, Card *out, int max_count);

/* 交易记录 */
int  record_add(const Record *r);
int  record_find_by_card(const char *card_no, Record *out, int max_count);
int  record_find_by_student(const char *student_no, Record *out, int max_count);
int  record_find_all(Record *out, int max_count);
int  record_search(const char *keyword, Record *out, int max_count);

/* 统计 */
int  stats_get(SystemStats *stats);

/* 获取下一个 ID */
int  get_next_student_id(void);
int  get_next_card_id(void);
int  get_next_record_id(void);

#ifdef __cplusplus
}
#endif

#endif /* CARD_DATA_H */
