#ifndef LOGIN_H
#define LOGIN_H

#include "card_data.h"
#include"file_io.h"
#include"utils.h"

// ====================== 登录模块函数 ======================
/**
 * @brief 学生登录校验
 * @param cardFile 学生卡数据文件
 * @param inputPwd 用户输入的密码
 * @return int 1-登录成功 0-密码错误 -1-账户冻结/挂失
 */
int StudentLogin(const char *cardFile, const char *inputPwd);

/**
 * @brief 管理员登录校验
 * @param inputPwd 管理员输入密码
 * @return int 1-登录成功 0-密码错误
 */
int AdminLogin(const char *inputPwd);
#endif