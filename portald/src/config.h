#pragma once

typedef struct {
    char wx_appid[128];
    char wx_secret[128];
    char wx_redirect[256];

    char sms_script[256];
    char db_path[256];

    int port;
    int session_timeout;

    int enable_passwd;
    int enable_sms;
    int enable_wechat;

    char success_redirect[256];

    char ip_whitelist[512];
    char mac_whitelist[512];

} portal_config_t;

extern portal_config_t g_cfg;

int load_config(const char *path);
void config_apply_defaults(void);
