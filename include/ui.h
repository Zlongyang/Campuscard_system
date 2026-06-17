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
 * @return int 用户选择的菜单项（1-学生登录 2-管理员登录 0-退出）
 */
int ShowMainMenu(void);

/**
 * @brief 学生功能菜单
 * @return int 用户选择的菜单项（1-查询信息 2-统计上机记录 0-退出）
 * @brief 功能：查询信息、统计上机记录、退出
 */
int StudentMenu(void);

/**
 * @brief 管理员功能菜单
 * @return int 用户选择的菜单项（1-充值 2-挂失卡片 3-重置密码 4-统计收入 0-退出）
 * @brief 功能：充值、挂失卡片、重置密码、统计收入、退出
 */
int AdminMenu(void);


#endif