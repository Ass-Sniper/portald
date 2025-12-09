#include "util.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/********************************************************
 * URL decode: in-place
 ********************************************************/
void url_decode(char *s)
{
    char *o = s;
    char hex[3] = {0};

    while (*s) {
        if (*s == '+') {
            *o++ = ' ';
            s++;
        } else if (*s == '%' && isxdigit((unsigned char)s[1]) &&
                   isxdigit((unsigned char)s[2])) {
            hex[0] = s[1];
            hex[1] = s[2];
            *o++ = (char)strtol(hex, NULL, 16);
            s += 3;
        } else {
            *o++ = *s++;
        }
    }
    *o = '\0';
}

/********************************************************
 * form_get_value: parse "a=b&c=d..." and return pointer
 * 注意：在原字符串上插入 '\0'，因此传入的 buffer 必须可写
 ********************************************************/
char *form_get_value(char *q, const char *key)
{
    if (!q || !key) return NULL;

    size_t klen = strlen(key);

    char *p = q;
    while (p && *p) {
        char *amp = strchr(p, '&');
        if (amp) *amp = '\0';

        char *eq = strchr(p, '=');
        if (eq) {
            *eq = '\0';
            char *k = p;
            char *v = eq + 1;

            if (!strcmp(k, key))
                return v;
        }

        if (!amp) break;
        p = amp + 1;
    }

    return NULL;
}

/********************************************************
 * SMS code in-memory store (simple, fixed size)
 ********************************************************/
typedef struct {
    char phone[32];
    char code[8];
    time_t expire_at;
} sms_entry_t;

#define MAX_SMS_ENTRIES 128
static sms_entry_t g_sms[MAX_SMS_ENTRIES];

int store_sms_code(const char *phone, const char *code, int ttl_seconds)
{
    time_t now = time(NULL);
    time_t exp = now + ttl_seconds;

    int idx = -1;

    // 覆盖旧的 or 找空位
    for (int i = 0; i < MAX_SMS_ENTRIES; ++i) {
        if (g_sms[i].phone[0] == 0 ||
            strcmp(g_sms[i].phone, phone) == 0) {
            idx = i;
            break;
        }
    }

    if (idx < 0)
        return -1;

    snprintf(g_sms[idx].phone, sizeof(g_sms[idx].phone), "%s", phone);
    snprintf(g_sms[idx].code,  sizeof(g_sms[idx].code),  "%s", code);
    g_sms[idx].expire_at = exp;

    return 0;
}

int verify_sms_code(const char *phone, const char *code)
{
    time_t now = time(NULL);

    for (int i = 0; i < MAX_SMS_ENTRIES; ++i) {
        if (!g_sms[i].phone[0])
            continue;

        if (strcmp(g_sms[i].phone, phone) == 0) {
            if (g_sms[i].expire_at < now) {
                // 过期
                g_sms[i].phone[0] = 0;
                g_sms[i].code[0] = 0;
                return -1;
            }
            if (strcmp(g_sms[i].code, code) == 0) {
                // 验证成功，清除
                g_sms[i].phone[0] = 0;
                g_sms[i].code[0] = 0;
                return 0;
            }
            return -1;
        }
    }

    return -1;
}