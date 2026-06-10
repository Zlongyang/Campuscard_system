/**
 * file_io.c — 文件读写层实现
 * 使用管道分隔符 "|" 的 CSV 格式存储数据
 * 格式:
 *   students.txt:  id|student_no|name|department|grade|phone|password_hash
 *   cards.txt:     id|card_no|student_no|name|balance|status|daily_limit|issue_date
 *   records.txt:   id|card_no|student_no|type|amount|balance_before|balance_after|time|description
 *   next_ids.dat:  student_id|card_id|record_id
 */
#include "include/file_io.h"
#include "include/utils.h"
#include <sys/stat.h>
#include <direct.h>

#ifdef _WIN32
    #define MKDIR(path) _mkdir(path)
#else
    #define MKDIR(path) mkdir(path, 0755)
#endif

/* ========== 内部辅助函数 ========== */

static int file_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0);
}

/* 分割一行（按 | 分隔）*/
static int split_line(char *line, char **fields, int max_fields) {
    int count = 0;
    char *p = line;
    fields[count++] = p;

    while (*p && count < max_fields) {
        if (*p == '|') {
            *p = '\0';
            p++;
            fields[count++] = p;
        } else if (*p == '\n' || *p == '\r') {
            *p = '\0';
            break;
        } else {
            p++;
        }
    }
    /* 去除末尾换行符 */
    if (*p == '\n' || *p == '\r') *p = '\0';
    return count;
}

/* ========== 目录和文件初始化 ========== */

int file_ensure_data_dir(void) {
    struct stat st;
    if (stat("data", &st) != 0) {
        return MKDIR("data");
    }
    return 0;
}

int file_ensure_all(void) {
    file_ensure_data_dir();

    /* 创建空文件（如果不存在） */
    const char *files[] = {
        FILE_STUDENTS, FILE_CARDS, FILE_RECORDS, "data/next_ids.dat", NULL
    };
    for (int i = 0; files[i]; i++) {
        if (!file_exists(files[i])) {
            FILE *fp = fopen(files[i], "w");
            if (fp) fclose(fp);
        }
    }

    /* 如果 ID 计数器文件为空，初始化它 */
    struct stat st;
    if (stat("data/next_ids.dat", &st) == 0 && st.st_size == 0) {
        file_save_next_ids(1, 1, 1);
    }

    return 0;
}

/* ========== 学生文件操作 ========== */

int file_load_students(Student *out, int max_count) {
    FILE *fp = fopen(FILE_STUDENTS, "r");
    if (!fp) return 0;

    char line[MAX_LINE_LEN];
    char *fields[8];
    int count = 0;

    while (fgets(line, sizeof(line), fp) && count < max_count) {
        int n = split_line(line, fields, 8);
        if (n < 7) continue; /* 跳过格式错误的行 */

        out[count].id = atoi(fields[0]);
        str_safe_copy(out[count].student_no, fields[1], sizeof(out[count].student_no));
        str_safe_copy(out[count].name, fields[2], sizeof(out[count].name));
        str_safe_copy(out[count].department, fields[3], sizeof(out[count].department));
        str_safe_copy(out[count].grade, fields[4], sizeof(out[count].grade));
        str_safe_copy(out[count].phone, fields[5], sizeof(out[count].phone));
        str_safe_copy(out[count].password, fields[6], sizeof(out[count].password));

        count++;
    }
    fclose(fp);
    return count;
}

int file_save_students(const Student *data, int count) {
    FILE *fp = fopen(FILE_STUDENTS, "w");
    if (!fp) return -1;

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d|%s|%s|%s|%s|%s|%s\n",
                data[i].id,
                data[i].student_no,
                data[i].name,
                data[i].department,
                data[i].grade,
                data[i].phone,
                data[i].password);
    }
    fclose(fp);
    return count;
}

int file_append_student(const Student *s) {
    FILE *fp = fopen(FILE_STUDENTS, "a");
    if (!fp) return -1;

    fprintf(fp, "%d|%s|%s|%s|%s|%s|%s\n",
            s->id, s->student_no, s->name,
            s->department, s->grade, s->phone, s->password);
    fclose(fp);
    return 0;
}

/* ========== 卡文件操作 ========== */

int file_load_cards(Card *out, int max_count) {
    FILE *fp = fopen(FILE_CARDS, "r");
    if (!fp) return 0;

    char line[MAX_LINE_LEN];
    char *fields[9];
    int count = 0;

    while (fgets(line, sizeof(line), fp) && count < max_count) {
        int n = split_line(line, fields, 9);
        if (n < 8) continue;

        out[count].id = atoi(fields[0]);
        str_safe_copy(out[count].card_no, fields[1], sizeof(out[count].card_no));
        str_safe_copy(out[count].student_no, fields[2], sizeof(out[count].student_no));
        str_safe_copy(out[count].name, fields[3], sizeof(out[count].name));
        out[count].balance = atof(fields[4]);
        out[count].status = atoi(fields[5]);
        out[count].daily_limit = atof(fields[6]);
        str_safe_copy(out[count].issue_date, fields[7], sizeof(out[count].issue_date));

        count++;
    }
    fclose(fp);
    return count;
}

int file_save_cards(const Card *data, int count) {
    FILE *fp = fopen(FILE_CARDS, "w");
    if (!fp) return -1;

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d|%s|%s|%s|%.2f|%d|%.2f|%s\n",
                data[i].id,
                data[i].card_no,
                data[i].student_no,
                data[i].name,
                data[i].balance,
                data[i].status,
                data[i].daily_limit,
                data[i].issue_date);
    }
    fclose(fp);
    return count;
}

int file_append_card(const Card *c) {
    FILE *fp = fopen(FILE_CARDS, "a");
    if (!fp) return -1;

    fprintf(fp, "%d|%s|%s|%s|%.2f|%d|%.2f|%s\n",
            c->id, c->card_no, c->student_no, c->name,
            c->balance, c->status, c->daily_limit, c->issue_date);
    fclose(fp);
    return 0;
}

/* ========== 记录文件操作 ========== */

int file_load_records(Record *out, int max_count) {
    FILE *fp = fopen(FILE_RECORDS, "r");
    if (!fp) return 0;

    char line[MAX_LINE_LEN * 2];
    char *fields[10];
    int count = 0;

    while (fgets(line, sizeof(line), fp) && count < max_count) {
        int n = split_line(line, fields, 10);
        if (n < 9) continue;

        out[count].id = atoi(fields[0]);
        str_safe_copy(out[count].card_no, fields[1], sizeof(out[count].card_no));
        str_safe_copy(out[count].student_no, fields[2], sizeof(out[count].student_no));
        out[count].type = atoi(fields[3]);
        out[count].amount = atof(fields[4]);
        out[count].balance_before = atof(fields[5]);
        out[count].balance_after = atof(fields[6]);
        str_safe_copy(out[count].time_str, fields[7], sizeof(out[count].time_str));
        str_safe_copy(out[count].description, fields[8], sizeof(out[count].description));

        count++;
    }
    fclose(fp);
    return count;
}

int file_save_records(const Record *data, int count) {
    FILE *fp = fopen(FILE_RECORDS, "w");
    if (!fp) return -1;

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d|%s|%s|%d|%.2f|%.2f|%.2f|%s|%s\n",
                data[i].id,
                data[i].card_no,
                data[i].student_no,
                data[i].type,
                data[i].amount,
                data[i].balance_before,
                data[i].balance_after,
                data[i].time_str,
                data[i].description);
    }
    fclose(fp);
    return count;
}

int file_append_record(const Record *r) {
    FILE *fp = fopen(FILE_RECORDS, "a");
    if (!fp) return -1;

    fprintf(fp, "%d|%s|%s|%d|%.2f|%.2f|%.2f|%s|%s\n",
            r->id, r->card_no, r->student_no, r->type,
            r->amount, r->balance_before, r->balance_after,
            r->time_str, r->description);
    fclose(fp);
    return 0;
}

/* ========== ID 计数器 ========== */

int file_load_next_ids(int *student_id, int *card_id, int *record_id) {
    FILE *fp = fopen("data/next_ids.dat", "r");
    if (!fp) {
        *student_id = 1;
        *card_id = 1;
        *record_id = 1;
        return 0;
    }

    char line[128];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        *student_id = 1;
        *card_id = 1;
        *record_id = 1;
        return 0;
    }

    char *fields[4];
    int n = split_line(line, fields, 4);
    *student_id = (n >= 1) ? atoi(fields[0]) : 1;
    *card_id     = (n >= 2) ? atoi(fields[1]) : 1;
    *record_id   = (n >= 3) ? atoi(fields[2]) : 1;

    fclose(fp);
    return 0;
}

int file_save_next_ids(int student_id, int card_id, int record_id) {
    FILE *fp = fopen("data/next_ids.dat", "w");
    if (!fp) return -1;

    fprintf(fp, "%d|%d|%d\n", student_id, card_id, record_id);
    fclose(fp);
    return 0;
}
