#include"login.h"

/************************ 登录模块函数实现 ************************/
/**
 * @brief 学生登录校验
 * @return int 1-登录成功 0-登录取消 -1-账户冻结/挂失/密码错误
 */
int StudentLogin(void)
{
    char inputPwd[MAX_ID_LEN] = {0};
    if (ConfigureStudentLogin() == -1)
    {
        return 0; // 登录取消
    }
    else
    {
        printf("请输入登录密码：");
        scanf("%s", inputPwd);
        ClearInput();

        // 判断账户状态：挂失/冻结直接禁止登录
        if (g_stuCard.state == CARD_LOST)
        {
            printf("账户已挂失，禁止登录！\n");
            Sleep(1000); // 等待1秒后返回主菜单
            return -1;
        }
        if (g_stuCard.state == CARD_FROZEN)
        {
            printf("账户已冻结，请联系管理员！\n");
            Sleep(1000); // 等待1秒后返回主菜单
            return -1;
        }

        // 密码比对
        if (strcmp(g_stuCard.pwd, inputPwd) == 0)
        {
            g_loginErrorCnt = 0; // 登录成功，清空错误次数
            printf("登录成功！\n");
            Sleep(1000); // 等待1秒后进入学生菜单
            return 1;
        }
        else
        {
            g_loginErrorCnt++; // 错误次数+1
            printf("密码错误！当前错误次数：%d/%d\n", g_loginErrorCnt, LOGIN_TIMES);
            Sleep(1000); // 等待1秒后返回主菜单
            // 错误次数达到3次，冻结账户
            if (g_loginErrorCnt >= LOGIN_TIMES)
            {
                g_stuCard.state = CARD_FROZEN;
                WriteCardInfo(g_student_card_file, &g_stuCard); // 更新冻结状态到文件
                printf("错误次数超限，账户已自动冻结！\n");
                Sleep(1000); // 等待1秒后返回主菜单
                return -1;
            }
            return -1;
        }
    }  
}

/**
 * @brief 管理员登录校验
 * @return int 1-登录成功 0-登录取消 -1-密码错误
 */
int AdminLogin(void)
{
    char inputPwd[MAX_ID_LEN] = {0};
    printf("========== 管理员登录 ==========\n");
    printf("请输入管理员密码：");
    scanf("%s", inputPwd);
    ClearInput();
    if (strcmp(ADMIN_PWD, inputPwd) == 0)
    {
        printf("管理员登录成功！\n");
        return 1;
    }
    else
    {
        printf("管理员密码错误！\n");
        Sleep(1000); // 等待1秒后返回主菜单
        return -1;
    }
}

/**
 * @brief 学生登录成功后记录上机开始时间，匹配卡号，设置上机状态(地点默认为上一次上机地点，后续可根据需求修改)
 * @param cardFile 学生卡数据文件路径
 * @param recordFile 学生上机记录文件路径
 */
void Loginrecord(const char *cardFile, const char *recordFile)
{
    // 获取当前系统时间作为上机开始时间
    StoreCurrentTime(&g_startTime);

    // 设置上机记录信息
    strcpy(g_stuRecord.ID, g_stuCard.ID); // 卡号
    sprintf(g_stuRecord.date, "%04d%02d%02d", g_startTime.tm_year + 1900, g_startTime.tm_mon + 1, g_startTime.tm_mday); // 日期 格式YYYYMMDD
    sprintf(g_stuRecord.start_time, "%02d:%02d", g_startTime.tm_hour, g_startTime.tm_min); // 开始时间 格式HH:MM
    g_stuRecord.duration = 0; // 初始用时为0
    g_stuRecord.online_state = ONLINE_YES; // 上机状态设置为正在上机
    // 上机地点保留上一次记录中的地址（已在 ResolveStudentPaths 中通过 ReadRecord 读入 g_stuRecord）
    // 如果地址为空（首次登录无历史记录），设置默认值
    if (strlen(g_stuRecord.address) == 0)
    {
        strcpy(g_stuRecord.address, "Unknown");
    }
}

/**
 * @brief 学生下机记录，计算上机时长和费用，更新卡片余额和上机记录,并追加记录到个人文件和全局文件
 * @param cardFile 学生卡数据文件路径
 * @param recordFile 学生上机记录文件路径
 */
void LogoutRecord(const char *cardFile, const char *recordFile)
{
    // 获取当前系统时间作为下机时间
    StoreCurrentTime(&g_endTime);

    // 计算上机时长和费用
    g_stuRecord.duration = CalculateDuration(&g_startTime, &g_endTime); // 计算上机时长（分钟）
    g_stuRecord.fee = g_stuRecord.duration * PER_MIN_FEE;              // 计算并存储单次费用
    // 更新卡片余额和上机记录
    g_stuCard.remain -= g_stuRecord.fee; // 扣除费用
    g_stuRecord.online_state = ONLINE_NO; // 上机状态设置为未上机
    // 更新卡片信息文件
    WriteCardInfo(cardFile, &g_stuCard);

    // 仅当实际产生了上机时长时才记录（跳过立即注销的零时长记录）
    if (g_stuRecord.duration > 0)
    {
        AppendRecord(recordFile, &g_stuRecord);
        AppendTotalRecord(TOTAL_RECORDS_FILE, &g_stuRecord);
    }
}

/**
 * @brief 输入学号验证并配置对应校园卡数据及路径
 * @return int 1-配置成功 -1-用户取消
 */
int ConfigureStudentLogin(void){
    char inputID[MAX_ID_LEN] = {0};
    do{        
        printf("请输入学号：（输入#退出）");
        scanf("%s", inputID);
        ClearInput();    
        if (strcmp(inputID, "#") == 0)
        {
            printf("正在返回主菜单...\n");
            Sleep(1000); // 等待1秒后返回主菜单
            return -1;
        } 
    }while(ResolveStudentPaths(inputID, g_student_card_file, g_record_file) == -1);
    return 1;
}

/**
 * @brief ConfigureStudentLogin函数的包装，做判断
 */
void ConfigureStudentLoginWrapper(void)
{
    system("cls");
    if(ConfigureStudentLogin() == -1){
        return; // 管理员登录取消
    }
    else
    {
        printf("已定位到学生数据文件，进入管理员菜单...\n");
        Sleep(1000); // 等待1秒后进入管理员菜单
    }
}

/**
 * @brief 登录失败提示并暂停，供 main.c 复用
 */
void HandleLoginFailed(void)
{
    printf("登录失败！按回车键返回主菜单...");
    getchar();
    Sleep(1000);
}