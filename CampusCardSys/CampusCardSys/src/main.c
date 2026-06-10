/**
 * main.c — 校园卡管理系统入口
 *
 * 编译方法 (MSVC):
 *   cl /nologo /W4 /I. /Ithirdparty src\*.c thirdparty\mongoose.c /Fe:CampusCardSys.exe
 *
 * 编译方法 (GCC/MinGW):
 *   gcc -Wall -Wextra -I. -Ithirdparty src/ *.c thirdparty/mongoose.c -o CampusCardSys.exe -lws2_32
 */
#include "include/server.h"
#include "include/login.h"
#include "include/file_io.h"
#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

/* ========== 启动横幅 ========== */

static void print_banner(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════╗\n");
    printf("║      🎓 校 园 卡 管 理 系 统  v1.0      ║\n");
    printf("║       Campus Card Management System      ║\n");
    printf("╚══════════════════════════════════════════╝\n");
    printf("\n");
}

static void print_usage(void) {
    printf("用法: CampusCardSys [端口号]\n");
    printf("示例: CampusCardSys 8080\n");
    printf("默认端口: 8080\n\n");
}

/* ========== 信号处理 ========== */

#ifdef _WIN32
#include <windows.h>
static BOOL WINAPI signal_handler(DWORD dwCtrlType) {
    if (dwCtrlType == CTRL_C_EVENT || dwCtrlType == CTRL_BREAK_EVENT) {
        printf("\n[SERVER] 正在关闭服务...\n");
        server_stop();
        return TRUE;
    }
    return FALSE;
}
#else
#include <signal.h>
static void signal_handler(int sig) {
    printf("\n[SERVER] 正在关闭服务...\n");
    server_stop();
}
#endif

static void setup_signal_handlers(void) {
#ifdef _WIN32
    SetConsoleCtrlHandler(signal_handler, TRUE);
#else
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
#endif
}

/* ========== 初始化 ========== */

static int init_system(void) {
    /* 初始化随机数种子 */
    srand((unsigned int)time(NULL));

    /* 确保数据目录和文件存在 */
    printf("[INIT] 初始化数据文件...\n");
    file_ensure_all();

    /* 初始化登录系统 */
    printf("[INIT] 初始化登录系统...\n");
    login_init();

    printf("[INIT] 系统初始化完成\n");
    return 0;
}

/* ========== 主函数 ========== */

int main(int argc, char *argv[]) {
    print_banner();

    /* 解析端口 */
    const char *port = SERVER_PORT;
    if (argc > 1) {
        port = argv[1];
        int p = atoi(port);
        if (p <= 0 || p > 65535) {
            fprintf(stderr, "[ERROR] 无效端口号: %s\n", port);
            print_usage();
            return 1;
        }
    }

    /* 初始化 */
    if (init_system() != 0) {
        fprintf(stderr, "[ERROR] 系统初始化失败\n");
        return 1;
    }

    /* 设置信号处理 */
    setup_signal_handlers();

    /* 启动服务器 */
    int ret = server_start(port);

    printf("[SERVER] 服务已停止\n");
    return ret;
}
