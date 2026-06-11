/*************************************************
* 文件名：card_data.h
* 功能：模拟校园卡消费查询系统 数据结构与函数声明头文件
* 说明：包含校园卡结构体、上机记录结构体、宏定义、全局变量、函数声明
**************************************************/
#ifndef CARD_DATA_H
#define CARD_DATA_H

// 引入基础头文件
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/************************ 宏定义 ************************/
#define MAX_ID_LEN      10      // 卡号、学号、姓名最大长度
#define MAX_ADDR_LEN    10      // 上机地点最大长度
#define LOGIN_TIMES     3       // 登录最大错误次数（3次错误冻结账户）
#define ADMIN_PWD       "admin" // 管理员默认密码
//业务数据常量
#define PER_MIN_FEE     0.15                 // 上机单价：0.15元/分钟

// 目前将学生校园卡数据文件和上机记录文件路径定义为常量，后续可根据需要修改为配置文件读取
#define STU_CARD_FILE   "data\\B25010101\\student_card.txt"   // 学生校园卡数据文件
#define RECORD_FILE     "data\\B25010101\\record.txt"      // 学生上机记录文件


// 校园卡状态枚举（对应state字段，增强代码可读性）
#define CARD_NORMAL     0       // 卡片正常状态
#define CARD_LOST       1       // 卡片挂失状态
#define CARD_FROZEN     2       // 卡片冻结状态

// 上机状态枚举
#define ONLINE_NO       0       // 未上机
#define ONLINE_YES      1       // 正在上机

/************************ 结构体定义 ************************/
/**
 * @brief 校园卡信息结构体
 * @param ID 卡号，长度固定10位
 * @param name 学生姓名
 * @param number 学生学号
 * @param pwd 学生登录密码
 * @param recharge 累计充值总额
 * @param remain 卡片当前余额
 * @param state 卡片状态：0正常/1挂失/2冻结
 */
typedef struct card
{
    char ID[MAX_ID_LEN];        // 卡号
    char name[MAX_ID_LEN];      // 姓名
    char number[MAX_ID_LEN];    // 学号
    char pwd[MAX_ID_LEN];       // 登录密码
    double recharge;            // 累计充值数额
    double remain;              // 当前余额
    int state;                  // 卡片状态 0-正常 1-挂失 2-冻结
} Card;

/**
 * @brief 上机记录结构体（每个学生单独文本文件存储）
 * @param ID 卡号
 * @param date 上机日期（格式示例：20260610）
 * @param start_time 上机开始时间（格式示例：2200）
 * @param duration 上机时长(分钟)
 * @param online_state 上机状态：0未上机/1上机中
 * @param address 上机地点（机房编号）
 */
typedef struct record
{
    char ID[MAX_ID_LEN];        // 卡号
    char date[9];               // 上机日期 格式YYYYMMDD
    char start_time[7];         // 开始时间 格式HH:MM
    int duration;               // 上机用时(分钟)
    char online_state;          // 上机状态 0-未上机 1-上机中
    char address[MAX_ADDR_LEN]; // 上机地点
} Record;

/************************ 全局变量声明 ************************/
extern Card g_stuCard;          // 当前登录学生的校园卡信息
extern int g_loginErrorCnt;     // 学生登录错误次数


#endif // CARD_DATA_H