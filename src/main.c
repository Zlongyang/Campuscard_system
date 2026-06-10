/*************************************************
* 文件名：main.c
* 功能：模拟校园卡消费查询系统 主程序
* 说明：实现系统菜单、角色选择、交互逻辑，调用底层功能函数
* 开发环境：VC++6.0 / Dev-C++ / VS 均可运行
**************************************************/

#include"card_data.h"
#include"file_io.h"
#include"utils.h"
#include"ui.h"

/*************************************************
* 主函数：程序入口
**************************************************/
int main(void)
{
    int mainChoice;
    char inputPwd[MAX_ID_LEN];

    // 初始化登录错误次数
    g_loginErrorCnt = 0;

    while (1)
    {
        // 显示主菜单
        ShowMainMenu();

        // 主菜单选择校验
        while (scanf("%d", &mainChoice) != 1)
        {
            ClearInput();
            printf("输入非法，请输入数字！重新选择：");
        }
        ClearInput();

        switch (mainChoice)
        {
            // 学生登录
            case 1:
                printf("========== 学生登录 ==========\n");
                printf("请输入登录密码：");
                scanf("%s", inputPwd);
                ClearInput();

                // 调用学生登录函数
                if (StudentLogin(STU_CARD_FILE, inputPwd) == 1)
                {
                    // 登录成功，进入学生菜单
                    StudentMenu();
                }
                else
                {
                    printf("登录失败！按回车键返回主菜单...");
                    getchar();
                }
                break;

            // 管理员登录
            case 2:
                printf("========== 管理员登录 ==========\n");
                printf("请输入管理员密码：");
                scanf("%s", inputPwd);
                ClearInput();

                if (AdminLogin(inputPwd) == 1)
                {
                    // 登录成功，进入管理员菜单
                    AdminMenu();
                }
                else
                {
                    printf("登录失败！按回车键返回主菜单...");
                    getchar();
                }
                break;

            // 退出系统
            case 0:
                printf("感谢使用，系统即将退出！\n");
                return 0;

            // 无效选项
            default:
                printf("选项不存在，请重新选择！\n");
                printf("按回车键继续...");
                getchar();
                break;
        }
    }
    return 0;
}