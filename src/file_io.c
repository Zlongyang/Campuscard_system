#include"file_io.h"

/************************ 基础文件操作函数实现 ************************/
/**
 * @brief 读取单个学生校园卡信息
 * @param cardFile 存储校园卡数据的文件名
 * @param card 接收读取结果的Card结构体指针
 * @return int 1-读取成功 0-读取失败
 */
int ReadCardInfo(const char *cardFile, Card *card)
{
    // 以只读模式打开校园卡数据文件
    FILE *fp = fopen(cardFile, "r");
    if (NULL == fp)
    {
        perror("打开校园卡文件失败");
        return 0;
    }

    // 格式化读取结构体所有字段，按文本格式解析
    int ret = fscanf(fp, "%s %s %s %s %lf %lf %d",
        card->ID,
        card->name,
        card->number,
        card->pwd,
        &card->recharge,
        &card->remain,
        &card->state);

    fclose(fp); // 关闭文件

    // 成功读取7个字段则返回成功
    return (ret == 7) ? 1 : 0;
}

/**
 * @brief 写入/更新学生校园卡信息到文件
 * @param cardFile 存储校园卡数据的文件名
 * @param card 待写入的Card结构体数据
 * @return int 1-写入成功 0-写入失败
 */
int WriteCardInfo(const char *cardFile, const Card *card)
{
    // 以覆盖写模式打开文件
    FILE *fp = fopen(cardFile, "w");
    if (NULL == fp)
    {
        perror("写入校园卡文件失败");
        return 0;
    }

    // 格式化写入结构体数据到文本文件
    fprintf(fp, "%s %s %s %s %.2lf %.2lf %d\n",
        card->ID,
        card->name,
        card->number,
        card->pwd,
        card->recharge,
        card->remain,
        card->state);

    fclose(fp);
    return 1;
}

/**
 * @brief 读取学生上机记录（个人txt文件）
 * @param recordFile 上机记录文件名（学号命名）
 * @param rec 接收读取结果的Record结构体指针
 * @return int 1-读取成功 0-读取失败
 */
int ReadRecord(const char *recordFile, Record *rec)
{
    FILE *fp = fopen(recordFile, "r");
    if (NULL == fp)
    {
        perror("打开上机记录文件失败");
        return 0;
    }

    int ret = fscanf(fp, "%s %s %s %d %c %s",
        rec->ID,
        rec->date,
        rec->start_time,
        &rec->duration,
        &rec->online_state,
        rec->address);

    fclose(fp);
    return (ret == 6) ? 1 : 0;
}

/**
 * @brief 追加一条新的上机记录到个人文件
 * @param recordFile 上机记录文件名
 * @param rec 待追加的Record结构体数据
 * @return int 1-追加成功 0-追加失败
 */
int AppendRecord(const char *recordFile, const Record *rec)
{
    // 以追加模式打开文件，在文件末尾写入数据
    FILE *fp = fopen(recordFile, "a");
    if (NULL == fp)
    {
        perror("追加上机记录失败");
        return 0;
    }

    fprintf(fp, "%s %s %s %d %c %s\n",
        rec->ID,
        rec->date,
        rec->start_time,
        rec->duration,
        rec->online_state,
        rec->address);

    fclose(fp);
    return 1;
}
