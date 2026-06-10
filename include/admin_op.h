#ifndef ADMIN_OP_H
#define ADMIN_OP_H

#include "card_data.h"
#include"file_io.h"
#include"utils.h"

// ====================== 管理员功能函数 ======================
/**
 * @brief 为学生校园卡充值
 * @param cardFile 学生卡数据文件
 * @param money 充值金额
 * @return int 1-充值成功 0-充值失败(卡片异常)
 */
int RechargeCard(const char *cardFile, double money);

/**
 * @brief 设置卡片为挂失状态
 * @param cardFile 学生卡数据文件
 * @return int 1-挂失成功 0-操作失败
 */
int LostCard(const char *cardFile);

/**
 * @brief 重置学生登录密码
 * @param cardFile 学生卡数据文件
 * @param newPwd 新密码
 * @return int 1-重置成功 0-重置失败
 */
int ResetStuPwd(const char *cardFile, const char *newPwd);

/**
 * @brief 统计当日全部上机收入（选做功能）
 * @return double 当日总收入金额
 */
double StatTodayIncome(void);
#endif