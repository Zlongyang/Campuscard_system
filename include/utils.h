#ifndef UTILS_H
#define UTILS_H

#include "card_data.h"
#include "file_io.h"

// ====================== 工具函数 ======================
/**
 * @brief 字符串输入合法性校验（防止空输入、超长）
 * @param str 待校验字符串
 * @param maxLen 字符串最大长度
 * @return int 1-合法 0-非法
 */
int CheckInput(char *str, int maxLen);

/**
 * @brief 清空输入缓冲区，处理scanf残留回车，避免连续输入异常
 * @brief 作用：处理scanf残留回车，避免连续输入异常
 */
void ClearInput(void);

/**
 * 储存当前系统时间
 * @param timePtr 指向struct tm的指针，函数将当前系统时间存储到该结构体中
 */
void StoreCurrentTime(struct tm *timePtr);

/**
 * 计算上机时长（分钟）
 * @param start 上机开始时间结构体指针
 * @param end 上机结束时间结构体指针
 * @return int 上机时长（分钟）
 */
int CalculateDuration(struct tm *start, struct tm *end);

/**
 * @brief 显示"无效选项"提示并暂停，供各 switch-default 复用
 */
void ShowInvalidOption(void);
#endif