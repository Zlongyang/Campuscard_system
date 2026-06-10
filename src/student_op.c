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
        printf("上机时长：%d 分钟\n", rec.duration);
        printf("上机状态：%s\n", (rec.online_state == ONLINE_YES) ? "上机中" : "未上机");
    }
    else
    {
        printf("暂无上机记录！\n");
    }
    printf("==================================\n");
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

    // 循环读取所有上机记录
    while (fscanf(fp, "%s %s %s %d %c %s",
        rec.ID, rec.date, rec.start_time, &rec.duration, &rec.online_state, rec.address) == 6)
    {
        totalTime += rec.duration;
    }
    fclose(fp);

    totalFee = totalTime * perMinFee;
    printf("========== 个人上机统计 ==========\n");
    printf("累计上机总时长：%d 分钟\n", totalTime);
    printf("上机单价：%.2lf 元/分钟\n", perMinFee);
    printf("累计上机总费用：%.2lf 元\n", totalFee);
    printf("=================================\n");
}
