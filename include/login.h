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
 * @return int 1-登录成功 0-登录取消 -1-账户冻结/挂失/密码错误
 */
int StudentLogin(void);

/**
 * @brief 管理员登录校验
 * @param inputPwd 管理员输入密码
 * @return int 1-登录成功 0-登录取消 -1-密码错误
 */
int AdminLogin(void);

/**
 * @brief 输入学号验证并配置对应校园卡数据及路径
 * @return int 1-配置成功 -1-用户取消登录
 */
int ConfigureStudentLogin(void);

/**
 * @brief ConfigureStudentLogin函数的包装，做判断
 */
void ConfigureStudentLoginWrapper(void);

/**
 * @brief 登录失败提示并暂停，供 main.c 复用
 */
void HandleLoginFailed(void);

/**
 * @brief 学生登录成功后记录上机开始时间，匹配卡号，设置上机状态
 * @param cardFile 学生卡数据文件路径
 * @param recordFile 学生上机记录文件路径
 */
void Loginrecord(const char *cardFile, const char *recordFile);

/**
 * @brief 学生下机记录，计算上机时长和费用，更新卡片余额和上机记录
 * @param cardFile 学生卡数据文件路径
 * @param recordFile 学生上机记录文件路径
 */
void LogoutRecord(const char *cardFile, const char *recordFile);

#endif