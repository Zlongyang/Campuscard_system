#ifndef FILE_IO_H
#define FILE_IO_H

#include "card_data.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========== 文件路径 ========== */
#define FILE_STUDENTS   "data/students.txt"
#define FILE_CARDS      "data/cards.txt"
#define FILE_RECORDS    "data/records.txt"

/* ========== 文件读写函数 ========== */

/* 确保数据文件存在 */
int  file_ensure_data_dir(void);
int  file_ensure_all(void);

/* 学生文件操作 */
int  file_load_students(Student *out, int max_count);
int  file_save_students(const Student *data, int count);
int  file_append_student(const Student *s);

/* 卡文件操作 */
int  file_load_cards(Card *out, int max_count);
int  file_save_cards(const Card *data, int count);
int  file_append_card(const Card *c);

/* 记录文件操作 */
int  file_load_records(Record *out, int max_count);
int  file_save_records(const Record *data, int count);
int  file_append_record(const Record *r);

/* ID 计数器 */
int  file_load_next_ids(int *student_id, int *card_id, int *record_id);
int  file_save_next_ids(int student_id, int card_id, int record_id);

#ifdef __cplusplus
}
#endif

#endif /* FILE_IO_H */
