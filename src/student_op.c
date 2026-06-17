#include"student_op.h"

/************************ 学生功能函数实现 ************************/
/**
 * @brief 查询卡片余额、上机地点、上机时间
 * @param recordFile 学生上机记录文件
 * @param card 学生校园卡信息
 */
void QueryStuInfo(const char *recordFile, const Card *card)
{
    Record rec;
    printf("========== 个人校园卡信息 ==========\n");
    printf("卡号：%s  姓名：%s  学号：%s\n", card->ID, card->name, card->number);
    printf("当前余额：%.2lf 元\n", card->remain);

    // 读取最新上机记录
    if (ReadRecord(recordFile, &rec))
    {
        printf("最近上机日期：%s\n", rec.date);
        printf("上机开始时间：%s\n", rec.start_time);
        printf("上机地点：%s\n", rec.address);
        printf("上机时长：%4d 分钟\n", rec.duration);
        printf("单次费用：%.2lf 元\n", rec.fee);
        printf("上机状态：%s\n", (rec.online_state == ONLINE_YES) ? "上机中" : "未上机");
    }
    else
    {
        printf("暂无上机记录！\n");
    }
    printf("==================================\n");
    printf("\n按回车键继续...");
    getchar();
}

/**
 * @brief 统计个人总上机时长、总上机费用
 * @param recordFile 学生上机记录文件
 * @param perMinFee 每分钟上机费用
 */
void StatStuRecord(const char *recordFile, double perMinFee)
{
    FILE *fp = fopen(recordFile, "r");
    if (NULL == fp)
    {
        printf("暂无上机记录，无法统计！\n");
        return;
    }

    Record rec;
    int totalTime = 0;      // 总上机时长(分钟)
    double totalFee = 0.0;  // 总上机费用

    int temp_online_state;
    // 循环读取所有上机记录
    while (fscanf(fp, "%s %s %s %d %d %s %lf",
        rec.ID, rec.date, rec.start_time, &rec.duration, &temp_online_state, rec.address, &rec.fee) == 7)
    {
        rec.online_state = (char)temp_online_state;
        totalTime += rec.duration;
    }
    fclose(fp);

    totalFee = totalTime * perMinFee;
    printf("========== 个人上机统计 ==========\n");
    printf("累计上机总时长：%4d 分钟\n", totalTime);
    printf("上机单价：%.2lf 元/分钟\n", perMinFee);
    printf("累计上机总费用：%.2lf 元\n", totalFee);
    printf("=================================\n");
    printf("\n按回车键继续...");
    getchar();
}

/**
 * @brief 列出个人全部上机记录
 * @param recordFile 学生上机记录文件
 */
void ListAllRecords(const char *recordFile)
{
    FILE *fp = fopen(recordFile, "r");
    if (NULL == fp)
    {
        printf("暂无上机记录！\n");
        return;
    }

    Record rec;
    int count = 0;
    int temp_online_state;

    printf("========== 个人上机记录列表 ==========\n");
    printf("%-6s %-10s %-7s %6s %-8s %-8s %-6s\n",
        "卡号", "日期", "开始时间", "时长", "状态", "地点", "费用");
    printf("-------------------------------------------------\n");

    while (fscanf(fp, "%s %s %s %d %d %s %lf",
        rec.ID, rec.date, rec.start_time,
        &rec.duration,
        &temp_online_state,
        rec.address,
        &rec.fee) == 7)
    {
        rec.online_state = (char)temp_online_state;
        count++;
        printf("%-6s %-10s %-7s %6d分 %-8s %-8s %5.2f元\n",
            rec.ID, rec.date, rec.start_time,
            rec.duration,
            (rec.online_state == ONLINE_YES) ? "上机中" : "未上机",
            rec.address,
            rec.fee);
    }

    fclose(fp);

    if (count == 0)
    {
        printf("暂无上机记录！\n");
    }
    else
    {
        printf("-------------------------------------------------\n");
        printf("共 %d 条记录\n", count);
    }
    printf("========================================\n");
    printf("\n按回车键继续...");
    getchar();
}
