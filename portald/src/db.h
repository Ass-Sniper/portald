#pragma once

#include <time.h>

/***************************************************************
 * 会话记录结构体：与 SQLite 表结构完全对应
 ***************************************************************/
typedef struct {
    int     id;             /* 自增主键 */
    char    ip[64];         /* 用户 IP */
    char    mac[64];        /* 用户 MAC */
    char    user[128];      /* 用户标识（用户名/手机号/OpenID） */
    time_t  login_time;     /* 登录时间戳 */
    time_t  logout_time;    /* 下线时间戳（0 表示仍在线） */
    char    reason[64];     /* 下线原因（kick/timeout/reboot/unknown） */
} session_rec_t;


/***************************************************************
 * 数据库初始化 / 关闭
 ***************************************************************/

/**
 * 初始化数据库，并自动创建 sessions 表。
 * @param path: 数据库路径
 * @return 0: 成功；非 0: 失败
 */
int db_init(const char *path);

/**
 * 关闭数据库
 */
void db_close(void);


/***************************************************************
 * 会话新增接口
 ***************************************************************/

/**
 * 添加新的在线会话（登录）
 * @param ip, mac, user: 会话信息
 * @param out_id: 返回自增 ID，可为 NULL
 * @return 0 成功，其他失败
 */
int db_add_session(const char *ip, const char *mac, const char *user, int *out_id);


/***************************************************************
 * 会话关闭接口（登出 / 被踢）
 ***************************************************************/

/**
 * 根据 IP+MAC 关闭会话（logout_time + reason）
 * @return 0 成功，其他失败
 */
int db_close_session(const char *ip, const char *mac, const char *reason);


/***************************************************************
 * 查询当前在线用户
 ***************************************************************/

/**
 * 获取所有未 logout 的会话（在线列表）
 * @param out_arr: 返回数组（需 free）
 * @param out_count: 数量
 */
int db_get_online(session_rec_t **out_arr, int *out_count);

/**
 * 获取需要从 DB 恢复的在线用户（重启后恢复用）
 * 语义与 db_get_online 一致。
 */
int db_get_online_for_restore(session_rec_t **out_arr, int *out_count);


/***************************************************************
 * 历史查询（按用户标识）
 ***************************************************************/

/**
 * 查询某个 user 的历史记录（含上线/下线时间）
 * 最多返回最近 1000 条
 */
int db_get_history_by_user(const char *user,
                           session_rec_t **out_arr, int *out_count);
