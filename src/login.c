#include"login.h"

/************************ 登录模块函数实现 ************************/
/**
 * @brief 学生登录校验
 * @param cardFile 学生卡数据文件
 * @param inputPwd 用户输入的密码
 * @return int 1-登录成功 0-密码错误 -1-账户冻结/挂失
 */
int StudentLogin(const char *cardFile, const char *inputPwd)
{
    // 先读取校园卡信息到全局变量
    if (!ReadCardInfo(cardFile, &g_stuCard))
    {
        printf("读取账户信息失败！\n");
        return -1;
    }

    // 判断账户状态：挂失/冻结直接禁止登录
    if (g_stuCard.state == CARD_LOST)
    {
        printf("账户已挂失，禁止登录！\n");
        return -1;
    }
    if (g_stuCard.state == CARD_FROZEN)
    {
        printf("账户已冻结，请联系管理员！\n");
        return -1;
    }

    // 密码比对
    if (strcmp(g_stuCard.pwd, inputPwd) == 0)
    {
        g_loginErrorCnt = 0; // 登录成功，清空错误次数
        printf("登录成功！\n");
        return 1;
    }
    else
    {
        g_loginErrorCnt++; // 错误次数+1
        printf("密码错误！当前错误次数：%d/%d\n", g_loginErrorCnt, LOGIN_TIMES);

        // 错误次数达到3次，冻结账户
        if (g_loginErrorCnt >= LOGIN_TIMES)
        {
            g_stuCard.state = CARD_FROZEN;
            WriteCardInfo(cardFile, &g_stuCard); // 更新冻结状态到文件
            printf("错误次数超限，账户已自动冻结！\n");
            return -1;
        }
        return 0;
    }
}

/**
 * @brief 管理员登录校验
 * @param inputPwd 管理员输入密码
 * @return int 1-登录成功 0-密码错误
 */
int AdminLogin(const char *inputPwd)
{
    if (strcmp(ADMIN_PWD, inputPwd) == 0)
    {
        printf("管理员登录成功！\n");
        return 1;
    }
    else
    {
        printf("管理员密码错误！\n");
        return 0;
    }
}