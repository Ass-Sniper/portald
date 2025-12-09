#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* 全局配置结构体实例 */
portal_config_t g_cfg;

/********************************************************
 * 工具函数：字符串 Trim（前后空白）
 ********************************************************/
static void trim(char *s)
{
    char *p = s;

    /* 前空白 */
    while (*p && isspace(*p)) p++;
    if (p != s)
        memmove(s, p, strlen(p) + 1);

    /* 后空白 */
    int len = strlen(s);
    while (len > 0 && isspace(s[len - 1]))
        s[--len] = 0;
}

/********************************************************
 * 默认值设置函数
 ********************************************************/
void config_apply_defaults(void)
{
    if (!g_cfg.wx_appid[0])
        strcpy(g_cfg.wx_appid, "default_appid");

    if (!g_cfg.wx_secret[0])
        strcpy(g_cfg.wx_secret, "default_secret");

    if (!g_cfg.wx_redirect[0])
        strcpy(g_cfg.wx_redirect, "http://192.168.16.254:8080/wx/callback");

    if (!g_cfg.sms_script[0])
        strcpy(g_cfg.sms_script, "/usr/bin/send_sms.sh");

    if (!g_cfg.db_path[0])
        strcpy(g_cfg.db_path, "/etc/portald/portal.db");

    if (g_cfg.session_timeout <= 0)
        g_cfg.session_timeout = 7200;   /* 2 hours */

    if (g_cfg.port <= 0)
        g_cfg.port = 8080;

    if (g_cfg.enable_passwd != 0 && g_cfg.enable_passwd != 1)
        g_cfg.enable_passwd = 1;

    if (g_cfg.enable_sms != 0 && g_cfg.enable_sms != 1)
        g_cfg.enable_sms = 1;

    if (g_cfg.enable_wechat != 0 && g_cfg.enable_wechat != 1)
        g_cfg.enable_wechat = 1;

    if (!g_cfg.success_redirect[0])
        strcpy(g_cfg.success_redirect, "https://www.bing.com");

    /* 白名单可为空 */
}

/********************************************************
 * 加载配置文件：格式 "key = value"
 ********************************************************/
int load_config(const char *path)
{
    memset(&g_cfg, 0, sizeof(g_cfg));

    FILE *f = fopen(path, "r");
    if (!f) {
        perror("open config");
        config_apply_defaults();
        return -1;
    }

    char line[512];

    while (fgets(line, sizeof(line), f)) {

        trim(line);

        /* 注释 或 空行 */
        if (line[0] == '#' || line[0] == 0)
            continue;

        char *eq = strchr(line, '=');
        if (!eq)
            continue;

        *eq = 0;
        char *key = line;
        char *val = eq + 1;

        trim(key);
        trim(val);

        /* WeChat OAuth */
        if (strcmp(key, "wx_appid") == 0) {
            strncpy(g_cfg.wx_appid, val, sizeof(g_cfg.wx_appid));
        }
        else if (strcmp(key, "wx_secret") == 0) {
            strncpy(g_cfg.wx_secret, val, sizeof(g_cfg.wx_secret));
        }
        else if (strcmp(key, "wx_redirect") == 0) {
            strncpy(g_cfg.wx_redirect, val, sizeof(g_cfg.wx_redirect));
        }

        /* SMS */
        else if (strcmp(key, "sms_script") == 0) {
            strncpy(g_cfg.sms_script, val, sizeof(g_cfg.sms_script));
        }

        /* Database */
        else if (strcmp(key, "db_path") == 0) {
            strncpy(g_cfg.db_path, val, sizeof(g_cfg.db_path));
        }

        /* Port */
        else if (strcmp(key, "port") == 0) {
            g_cfg.port = atoi(val);
        }

        /* Session timeout */
        else if (strcmp(key, "session_timeout") == 0) {
            g_cfg.session_timeout = atoi(val);
        }

        /* Authentication method switches */
        else if (strcmp(key, "enable_passwd") == 0) {
            g_cfg.enable_passwd = atoi(val);
        }
        else if (strcmp(key, "enable_sms") == 0) {
            g_cfg.enable_sms = atoi(val);
        }
        else if (strcmp(key, "enable_wechat") == 0) {
            g_cfg.enable_wechat = atoi(val);
        }

        /* Login success redirect URL */
        else if (strcmp(key, "success_redirect") == 0) {
            strncpy(g_cfg.success_redirect, val, sizeof(g_cfg.success_redirect));
        }

        /* Whitelist */
        else if (strcmp(key, "ip_whitelist") == 0) {
            strncpy(g_cfg.ip_whitelist, val, sizeof(g_cfg.ip_whitelist));
        }
        else if (strcmp(key, "mac_whitelist") == 0) {
            strncpy(g_cfg.mac_whitelist, val, sizeof(g_cfg.mac_whitelist));
        }
    }

    fclose(f);

    /* 自动补全默认值 */
    config_apply_defaults();

    return 0;
}
