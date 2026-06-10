#include"admin_op.h"

/************************ 管理员功能函数实现 ************************/
/**
 * @brief 为学生校园卡充值
 * @param cardFile 学生卡数据文件
 * @param money 充值金额
 * @return int 1-充值成功 0-充值失败(卡片异常)
 */
int RechargeCard(const char *cardFile, double money)
{
    // 读取卡片信息
    if (!ReadCardInfo(cardFile, &g_stuCard))
        return 0;

    // 挂失/冻结卡片禁止充值
    if (g_stuCard.state != CARD_NORMAL)
    {
        printf("卡片状态异常，无法充值！\n");
        return 0;
    }

    // 更新充值总额和余额
    g_stuCard.recharge += money;
    g_stuCard.remain += money;

    // 写入更新后的数据
    WriteCardInfo(cardFile, &g_stuCard);
    printf("充值成功！本次充值：%.2lf 元，当前余额：%.2lf 元\n", money, g_stuCard.remain);
    return 1;
}

/**
 * @brief 设置卡片为挂失状态
 * @param cardFile 学生卡数据文件
 * @return int 1-挂失成功 0-操作失败
 */
int LostCard(const char *cardFile)
{
    if (!ReadCardInfo(cardFile, &g_stuCard))
        return 0;

    // 已挂失无需重复操作
    if (g_stuCard.state == CARD_LOST)
    {
        printf("该卡片已处于挂失状态！\n");
        return 0;
    }

    g_stuCard.state = CARD_LOST;
    WriteCardInfo(cardFile, &g_stuCard);
    printf("卡片挂失操作完成！\n");
    return 1;
}

/**
 * @brief 重置学生登录密码
 * @param cardFile 学生卡数据文件
 * @param newPwd 新密码
 * @return int 1-重置成功 0-重置失败
 */
int ResetStuPwd(const char *cardFile, const char *newPwd)
{
    if (!ReadCardInfo(cardFile, &g_stuCard))
        return 0;

    // 更新密码
    strcpy(g_stuCard.pwd, newPwd);
    WriteCardInfo(cardFile, &g_stuCard);
    printf("学生密码重置成功！\n");
    return 1;
}

/**
 * @brief 统计当日全部上机收入（选做功能）
 * @return double 当日总收入金额
 */
double StatTodayIncome(void)
{
    // 简易实现：遍历所有学生记录，统计当日消费（可根据需求扩展多文件遍历）
    // 此处为框架预留，完整实现需批量读取所有学生txt记录
    double total = 0.0;
    printf("当日总收入统计功能（待扩展批量读取）\n");
    return total;
}