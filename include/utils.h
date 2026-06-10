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
#endif