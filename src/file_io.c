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
 * @brief 遍历并展示全局上机记录文件
 * @param totalRecordsFile 全局上机记录文件路径
 */
void CountStudentRecords(const char *totalRecordsFile)
{
    FILE *fp = fopen(totalRecordsFile, "r");
    if (NULL == fp)
    {
        perror("打开全局上机记录文件失败");
        return;
    }

    char line[256];
    printf("全局上机记录列表：\n");
    printf("卡号\t日期\t\t开始时间\t用时(分钟)\t上机状态\t上机地点\t费用(元)\n");
    while (fgets(line, sizeof(line), fp))
    {
        printf("%s", line); // 直接输出每行记录
    }

    fclose(fp);
    printf("\n按回车键继续...");
    getchar();
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

    Record temp;
    int found = 0;
    int temp_online_state;

    // 循环读取最后一条有效记录（跳过可能的损坏行）
    while (fscanf(fp, "%s %s %s %d %d %s %lf",
        temp.ID,
        temp.date,
        temp.start_time,
        &temp.duration,
        &temp_online_state,
        temp.address,
        &temp.fee) == 7)
    {
        temp.online_state = (char)temp_online_state;
        *rec = temp;   // 保存最后一条有效记录
        found = 1;
    }

    fclose(fp);
    return found;
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

    fprintf(fp, "%s %s %s %d %d %s %.2lf\n",
        rec->ID,
        rec->date,
        rec->start_time,
        rec->duration,
        (int)rec->online_state,
        rec->address,
        rec->fee);

    fclose(fp);
    return 1;
}

/**
 * @brief 追加一条新的上机记录到全局文件
 * @param totalRecordsFile 全局上机记录文件路径
 * @param rec 待追加的Record结构体数据
 * @return int 1-追加成功 0-追加失败
 */
int AppendTotalRecord(const char *totalRecordsFile, const Record *rec)
{
    // 以追加模式打开全局记录文件
    FILE *fp = fopen(totalRecordsFile, "a");
    if (NULL == fp)
    {
        perror("追加全局上机记录失败");
        return 0;
    }

    fprintf(fp, "%s %s %s %d %d %s %.2lf\n",
        rec->ID,
        rec->date,
        rec->start_time,
        rec->duration,
        (int)rec->online_state,
        rec->address,
        rec->fee);

    fclose(fp);
    return 1;
}

/************************ 路径解析函数实现 ************************/
/**
 * @brief 根据学生学号动态解析校园卡文件路径和上机记录文件路径
 * @param studentID 学生学号
 * @param cardPath 输出缓冲区，接收校园卡文件路径
 * @param recordPath 输出缓冲区，接收上机记录文件路径
 * @note 路径格式: "data\<studentID>\student_card.txt" 和 "data\<studentID>\record.txt"
 * @return int 1-解析成功 -1-解析失败
 */
int ResolveStudentPaths(const char *studentID, char *cardPath, char *recordPath)
{
    sprintf(cardPath, "data\\%s\\student_card.txt", studentID);
    sprintf(recordPath, "data\\%s\\record.txt", studentID);

    // 判断标准：校园卡文件能否成功读取
    if (!ReadCardInfo(cardPath, &g_stuCard))
    {
        printf("学号对应的校园卡数据不存在，请检查学号是否正确！\n");
        return -1;
    }

    // 上机记录允许为空（新学生无记录），不阻塞登录
    if (!ReadRecord(recordPath, &g_stuRecord))
    {
        printf("（该学生暂无上机记录）\n");
        g_stuRecord = (Record){0};
        strcpy(g_stuRecord.address, "Unknown");
    }
    return 1;
}
