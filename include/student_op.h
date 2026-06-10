#ifndef STUDENT_OP_H
#define STUDENT_OP_H

#include "card_data.h"
#include"file_io.h"
#include"utils.h"

// ====================== 学生功能函数 ======================
/**
 * @brief 查询卡片余额、上机地点、上机时间
 * @param recordFile 学生上机记录文件
 * @param card 学生校园卡信息
 */
void QueryStuInfo(const char *recordFile, const Card *card);

/**
 * @brief 统计个人总上机时长、总上机费用
 * @param recordFile 学生上机记录文件
 * @param perMinFee 每分钟上机费用
 */
void StatStuRecord(const char *recordFile, double perMinFee);
#endif