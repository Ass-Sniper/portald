#pragma once

void url_decode(char *s);
char *form_get_value(char *query_or_body, const char *key);

int store_sms_code(const char *phone, const char *code, int ttl_seconds);
int verify_sms_code(const char *phone, const char *code);