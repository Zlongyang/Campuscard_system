#ifndef ADMIN_OP_H
#define ADMIN_OP_H

#include "card_data.h"
#include"file_io.h"
#include"utils.h"

// ====================== 管理员功能函数 ======================
// 每个函数封装完整的交互流程：清屏 → (选择学生) → 执行操作 → 暂停

/**
 * @brief 充值工作流：选择学生 → 输入金额 → 充值 → 暂停
 */
void RechargeCard(void);

/**
 * @brief 挂失工作流：选择学生 → 挂失 → 暂停
 */
void LostCard(void);

/**
 * @brief 重置密码工作流：选择学生 → 输入新密码 → 重置 → 暂停
 */
void ResetStuPwd(void);

/**
 * @brief 统计当日全部上机收入 → 暂停
 */
void StatTodayIncome(void);

/**
 * @brief 解冻工作流：选择学生 → 解冻 → 暂停
 */
void UnfreezeCard(void);
#endif