#include "ui.h"



/*************************************************
* @brief 展示系统主菜单（角色选择）
**************************************************/
void ShowMainMenu(void)
{
    system("cls");  // 清屏，Windows环境；Linux请替换为 system("clear")
    printf("==================== 模拟校园卡消费查询系统 ====================\n");
    printf("                    1. 学生登录\n");
    printf("                    2. 管理员登录\n");
    printf("                    0. 退出系统\n");
    printf("================================================================\n");
    printf("请输入您的选择：");
}

/*************************************************
* @brief 学生功能菜单
* 功能：查询信息、统计上机记录、退出
**************************************************/
void StudentMenu(void)
{
    int choice;
    while (1)
    {
        system("cls");
        printf("==================== 学生功能菜单 ====================\n");
        printf("                1. 查询校园卡及上机信息\n");
        printf("                2. 统计个人上机时长与费用\n");
        printf("                0. 返回主菜单\n");
        printf("======================================================\n");
        printf("请选择功能：");

        // 输入校验，保证为数字
        while (scanf("%d", &choice) != 1)
        {
            ClearInput();
            printf("输入非法，请输入数字！请重新选择：");
        }
        ClearInput();

        switch (choice)
        {
            case 1:
                // 查询余额、上机地点、上机时间
                QueryStuInfo(RECORD_FILE, &g_stuCard);
                printf("\n按回车键继续...");
                getchar();
                break;
            case 2:
                // 统计上机时长和费用
                StatStuRecord(RECORD_FILE, PER_MIN_FEE);
                printf("\n按回车键继续...");
                getchar();
                break;
            case 0:
                printf("正在返回主菜单...\n");
                return;
            default:
                printf("输入选项无效，请重新选择！\n");
                printf("按回车键继续...");
                getchar();
                break;
        }
    }
}

/*************************************************
* @brief 管理员功能菜单
* 功能：充值、挂失卡片、重置密码、统计收入、退出
**************************************************/
void AdminMenu(void)
{
    int choice;
    char newPwd[MAX_ID_LEN];
    double money;

    while (1)
    {
        system("cls");
        printf("==================== 管理员功能菜单 ====================\n");
        printf("                1. 学生校园卡充值\n");
        printf("                2. 办理卡片挂失\n");
        printf("                3. 重置学生登录密码\n");
        printf("                4. 统计当日上机总收入\n");
        printf("                0. 返回主菜单\n");
        printf("========================================================\n");
        printf("请选择功能：");

        while (scanf("%d", &choice) != 1)
        {
            ClearInput();
            printf("输入非法，请输入数字！请重新选择：");
        }
        ClearInput();

        switch (choice)
        {
            case 1:
                printf("请输入充值金额：");
                while (scanf("%lf", &money) != 1 || money <= 0)
                {
                    ClearInput();
                    printf("金额输入非法，请输入正数！重新输入：");
                }
                ClearInput();
                RechargeCard(STU_CARD_FILE, money);
                printf("\n按回车键继续...");
                getchar();
                break;

            case 2:
                LostCard(STU_CARD_FILE);
                printf("\n按回车键继续...");
                getchar();
                break;

            case 3:
                printf("请输入新的学生密码：");
                scanf("%s", newPwd);
                ClearInput();
                // 密码合法性校验
                if (CheckInput(newPwd, MAX_ID_LEN - 1))
                {
                    ResetStuPwd(STU_CARD_FILE, newPwd);
                }
                printf("\n按回车键继续...");
                getchar();
                break;

            case 4:
                StatTodayIncome();
                printf("\n按回车键继续...");
                getchar();
                break;

            case 0:
                printf("正在返回主菜单...\n");
                return;

            default:
                printf("输入选项无效，请重新选择！\n");
                printf("按回车键继续...");
                getchar();
                break;
        }
    }
}