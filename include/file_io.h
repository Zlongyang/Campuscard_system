#ifndef FILE_IO_H
#define FILE_IO_H

#include "card_data.h"

// ====================== 基础文件操作函数 ======================
/**
 * @brief 读取单个学生校园卡信息
 * @param cardFile 存储校园卡数据的文件名
 * @param card 接收读取结果的Card结构体指针
 * @return int 1-读取成功 0-读取失败
 */
int ReadCardInfo(const char *cardFile, Card *card);

/**
 * @brief 写入/更新学生校园卡信息到文件
 * @param cardFile 存储校园卡数据的文件名
 * @param card 待写入的Card结构体数据
 * @return int 1-写入成功 0-写入失败
 */
int WriteCardInfo(const char *cardFile, const Card *card);

/**
 * @brief 读取学生上机记录（个人txt文件）
 * @param recordFile 上机记录文件名（学号命名）
 * @param rec 接收读取结果的Record结构体指针
 * @return int 1-读取成功 0-读取失败
 */
int ReadRecord(const char *recordFile, Record *rec);

/**
 * @brief 追加一条新的上机记录到个人文件
 * @param recordFile 上机记录文件名
 * @param rec 待追加的Record结构体数据
 * @return int 1-追加成功 0-追加失败
 */
int AppendRecord(const char *recordFile, const Record *rec);

#endif