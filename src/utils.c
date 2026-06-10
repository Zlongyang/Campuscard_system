#include "utils.h"


/************************ 工具函数实现 ************************/
/**
 * @brief 字符串输入合法性校验（防止空输入、超长）
 * @param str 待校验字符串
 * @param maxLen 字符串最大长度
 * @return int 1-合法 0-非法
 */
int CheckInput(char *str, int maxLen)
{
    // 去除首尾空格，判断是否为空
    if (strlen(str) == 0 || strlen(str) > maxLen)
    {
        printf("输入内容为空或长度超限！\n");
        return 0;
    }
    return 1;
}

/*************************************************
* @brief 清空输入缓冲区
* 作用：处理scanf残留回车，避免连续输入异常
**************************************************/
void ClearInput(void)
{
    while (getchar() != '\n');
}