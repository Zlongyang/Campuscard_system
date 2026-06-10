#ifndef SERVER_H
#define SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

/* ========== 服务器配置 ========== */
#define SERVER_PORT     "8080"
#define WEB_ROOT        "web"

/* ========== 函数声明 ========== */

/* 启动 HTTP 服务器（基于 Mongoose） */
int  server_start(const char *port);

/* 停止服务器 */
void server_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* SERVER_H */
