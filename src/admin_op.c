#include"admin_op.h"
#include"login.h"

/************************ 管理员功能函数实现 ************************/

/**
 * @brief 充值工作流：选择学生 → 输入金额 → 充值 → 暂停
 */
void RechargeCard(void)
{
    double money;
    system("cls");
    if (ConfigureStudentLogin() == -1) return;

    printf("请输入充值金额：");
    while (scanf("%lf", &money) != 1 || money <= 0)
    {
        ClearInput();
        printf("金额输入非法，请输入正数！重新输入：");
    }
    ClearInput();

    // 读取卡片信息
    if (!ReadCardInfo(g_student_card_file, &g_stuCard)) return;

    // 挂失/冻结卡片禁止充值
    if (g_stuCard.state != CARD_NORMAL)
    {
        printf("卡片状态异常，无法充值！\n");
        printf("\n按回车键继续...");
        getchar();
        return;
    }

    // 更新充值总额和余额
    g_stuCard.recharge += money;
    g_stuCard.remain += money;

    // 写入更新后的数据
    WriteCardInfo(g_student_card_file, &g_stuCard);
    printf("充值成功！本次充值：%.2lf 元，当前余额：%.2lf 元\n", money, g_stuCard.remain);
    printf("\n按回车键继续...");
    getchar();
}

/**
 * @brief 挂失工作流：选择学生 → 挂失 → 暂停
 */
void LostCard(void)
{
    system("cls");
    if (ConfigureStudentLogin() == -1) return;

    if (!ReadCardInfo(g_student_card_file, &g_stuCard)) return;

    // 已挂失无需重复操作
    if (g_stuCard.state == CARD_LOST)
    {
        printf("该卡片已处于挂失状态！\n");
        printf("\n按回车键继续...");
        getchar();
        return;
    }

    g_stuCard.state = CARD_LOST;
    WriteCardInfo(g_student_card_file, &g_stuCard);
    printf("卡片挂失操作完成！\n");
    printf("\n按回车键继续...");
    getchar();
}

/**
 * @brief 重置密码工作流：选择学生 → 输入新密码 → 重置 → 暂停
 */
void ResetStuPwd(void)
{
    char newPwd[MAX_ID_LEN];
    system("cls");
    if (ConfigureStudentLogin() == -1) return;

    printf("请输入新的学生密码：");
    scanf("%s", newPwd);
    ClearInput();
    if (CheckInput(newPwd, MAX_ID_LEN - 1))
    {
        if (ReadCardInfo(g_student_card_file, &g_stuCard))
        {
            // 更新密码
            strcpy(g_stuCard.pwd, newPwd);
            WriteCardInfo(g_student_card_file, &g_stuCard);
            printf("学生密码重置成功！\n");
        }
    }
    printf("\n按回车键继续...");
    getchar();
}

/**
 * @brief 统计当日全部上机收入 → 暂停
 */
void StatTodayIncome(void)
{
    double total = 0.0;
    char today[11];
    struct tm today_tm;
    time_t now = time(NULL);

    localtime_s(&today_tm, &now);
    sprintf(today, "%04d%02d%02d",
        today_tm.tm_year + 1900,
        today_tm.tm_mon + 1,
        today_tm.tm_mday);

    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile("data\\*", &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("无法读取数据目录！\n");
        printf("\n按回车键继续...");
        getchar();
        return;
    }

    printf("========== 当日上机收入统计 ==========\n");
    printf("日期：%s\n", today);
    printf("%-8s %-10s %-7s %-6s %-8s %-6s\n",
        "卡号", "日期", "开始时间", "时长", "地点", "费用");
    printf("----------------------------------------\n");

    do
    {
        // 跳过非目录和 . / ..
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;
        if (strcmp(findData.cFileName, ".") == 0 ||
            strcmp(findData.cFileName, "..") == 0)
            continue;

        // 拼接该学生目录下的 record.txt 路径
        char recordPath[256];
        sprintf(recordPath, "data\\%s\\record.txt", findData.cFileName);

        FILE *fp = fopen(recordPath, "r");
        if (NULL == fp) continue;

        Record rec;
        int temp_online;
        while (fscanf(fp, "%s %s %s %d %d %s %lf",
            rec.ID, rec.date, rec.start_time,
            &rec.duration,
            &temp_online,
            rec.address,
            &rec.fee) == 7)
        {
            if (strcmp(rec.date, today) == 0)
            {
                total += rec.fee;
                printf("%-8s %-10s %-7s %4d分 %-8s %5.2f元\n",
                    rec.ID, rec.date, rec.start_time,
                    rec.duration,
                    rec.address,
                    rec.fee);
            }
        }
        fclose(fp);
    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);

    printf("----------------------------------------\n");
    printf("当日总收入：%.2lf 元\n", total);
    printf("========================================\n");
    printf("\n按回车键继续...");
    getchar();
}

/**
 * @brief 解冻工作流：选择学生 → 解冻 → 暂停
 */
void UnfreezeCard(void)
{
    system("cls");
    if (ConfigureStudentLogin() == -1) return;

    if (!ReadCardInfo(g_student_card_file, &g_stuCard)) return;

    // 已解冻无需重复操作
    if (g_stuCard.state == CARD_NORMAL)
    {
        printf("该卡片已处于正常状态！\n");
        printf("\n按回车键继续...");
        getchar();
        return;
    }

    g_stuCard.state = CARD_NORMAL;
    WriteCardInfo(g_student_card_file, &g_stuCard);
    printf("学生账户解冻操作完成！\n");
    printf("\n按回车键继续...");
    getchar();
}