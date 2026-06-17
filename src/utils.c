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

/**
 * 储存当前系统时间
 * @param timePtr 指向struct tm的指针，函数将当前系统时间存储到该结构体中
 */
void StoreCurrentTime(struct tm *timePtr)
{
    time_t now = time(NULL); // 获取当前系统时间
    localtime_s(timePtr, &now); // 将当前时间转换为结构体形式并存储到timePtr指向的结构体中
}

/**
 * 计算上机时长（分钟）
 * @param start 上机开始时间结构体指针
 * @param end 上机结束时间结构体指针
 * @return int 上机时长（分钟）
 */
int CalculateDuration(struct tm *start, struct tm *end)
{
    time_t startTime = mktime(start); // 将开始时间转换为time_t类型
    time_t endTime = mktime(end);     // 将结束时间转换为time_t类型
    double durationSec = difftime(endTime, startTime); // 计算时间差，单位为秒
    int durationMin = (int)(durationSec / 60); // 将秒转换为分钟并取整
    return durationMin;
}

/**
 * @brief 显示"无效选项"提示并暂停，供各 switch-default 复用
 */
void ShowInvalidOption(void)
{
    printf("输入选项无效，请重新选择！\n");
    printf("按回车键继续...");
    getchar();
}