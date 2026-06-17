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
#include"login.h"
#include"admin_op.h"

/*************************************************
* 主函数：程序入口
**************************************************/
int main(void)
{
    // 初始化全局变量
    g_loginErrorCnt = 0;            // 初始化登录错误次数
    g_stuCard= (Card){0};           // 初始化学生卡信息结构体
    g_stuRecord = (Record){0};       // 初始化上机记录信息结构体
    g_student_card_file[0] = '\0';  // 初始化学生卡文件路径为空
    g_record_file[0] = '\0';        // 初始化上机记录文件路径
    g_startTime = (struct tm){0};   // 初始化上机开始时间
    g_endTime = (struct tm){0};     // 初始化上机结束时间

    while (1)
    {   
        // 显示主菜单
        int mainChoice = ShowMainMenu();
        switch (mainChoice)
        {
            // 学生登录
            case 1:
                if (StudentLogin() == 1)
                {
                    // 登录成功，记录上机开始时间，匹配卡号，设置上机状态
                    Loginrecord(g_student_card_file, g_record_file);
                    int studentChoice;
                    do
                    {
                        studentChoice = StudentMenu();
                        switch (studentChoice)
                        {
                            case 1: QueryStuInfo(g_record_file, &g_stuCard);     break;
                            case 2: StatStuRecord(g_record_file, PER_MIN_FEE);   break;
                            case 3: ListAllRecords(g_record_file);               break;
                            case 0:
                                LogoutRecord(g_student_card_file, g_record_file);
                                printf("正在返回主菜单...\n");
                            break;
                            default: ShowInvalidOption(); break;
                        }
                    } while (studentChoice);
                }
                break;

            // 管理员登录
            case 2:
                if (AdminLogin() == 1)
                {
                    // 登录成功，进入管理员菜单
                    int adminChoice;
                    do
                    {
                        adminChoice = AdminMenu();
                        switch (adminChoice)
                        {
                            case 1: RechargeCard();                  break;
                            case 2: LostCard();                      break;
                            case 3: ResetStuPwd();                   break;
                            case 4: StatTodayIncome();               break;
                            case 5: UnfreezeCard();                  break;
                            case 6: CountStudentRecords(TOTAL_RECORDS_FILE); break;
                            case 0:
                                printf("正在返回主菜单...\n");
                            break;
                            default: ShowInvalidOption(); break;
                        }
                    } while (adminChoice);
                }
                else
                {
                    HandleLoginFailed();
                }
                break;

            case 0:
                printf("感谢使用，系统即将退出！\n");
                return 0;

            default: ShowInvalidOption(); break;
        }
    }
    return 0;
}