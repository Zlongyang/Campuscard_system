#ifndef UI_H
#define UI_H

#include "card_data.h"
#include"file_io.h"
#include"utils.h"
#include"login.h"
#include"admin_op.h" 
#include"student_op.h"

//===================== UI界面函数 ======================

/**
 * @brief 展示系统主菜单（角色选择）
 */
void ShowMainMenu(void);

/**
 * @brief 学生功能菜单
 * @brief 功能：查询信息、统计上机记录、退出
 */
void StudentMenu(void);

/**
 * @brief 管理员功能菜单
 * @brief 功能：充值、挂失卡片、重置密码、统计收入、退出
 */
void AdminMenu(void);


#endif