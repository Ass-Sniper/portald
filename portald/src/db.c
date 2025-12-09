#include "db.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static sqlite3 *g_db = NULL;

int db_init(const char *path)
{
    if (sqlite3_open(path, &g_db) != SQLITE_OK) {
        fprintf(stderr, "db_init: open %s failed: %s\n", path, sqlite3_errmsg(g_db));
        return -1;
    }

    const char *sql =
        "CREATE TABLE IF NOT EXISTS sessions ("
        " id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " ip TEXT NOT NULL,"
        " mac TEXT NOT NULL,"
        " user TEXT NOT NULL,"
        " login_time INTEGER NOT NULL,"
        " logout_time INTEGER,"
        " reason TEXT"
        ");";

    char *errmsg = NULL;
    if (sqlite3_exec(g_db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
        fprintf(stderr, "db_init: create table failed: %s\n", errmsg);
        sqlite3_free(errmsg);
        return -1;
    }

    return 0;
}

void db_close(void)
{
    if (g_db) sqlite3_close(g_db);
    g_db = NULL;
}

int db_add_session(const char *ip, const char *mac, const char *user, int *out_id)
{
    const char *sql =
        "INSERT INTO sessions(ip, mac, user, login_time) VALUES (?,?,?,?);";

    sqlite3_stmt *st = NULL;
    if (sqlite3_prepare_v2(g_db, sql, -1, &st, NULL) != SQLITE_OK)
        return -1;

    time_t now = time(NULL);

    sqlite3_bind_text(st, 1, ip,   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, mac,  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, user, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 4, (sqlite3_int64)now);

    if (sqlite3_step(st) != SQLITE_DONE) {
        sqlite3_finalize(st);
        return -1;
    }

    if (out_id)
        *out_id = (int)sqlite3_last_insert_rowid(g_db);

    sqlite3_finalize(st);
    return 0;
}

int db_close_session(const char *ip, const char *mac, const char *reason)
{
    const char *sql =
        "UPDATE sessions SET logout_time=?, reason=? "
        "WHERE ip=? AND mac=? AND logout_time IS NULL;";

    sqlite3_stmt *st = NULL;
    if (sqlite3_prepare_v2(g_db, sql, -1, &st, NULL) != SQLITE_OK)
        return -1;

    time_t now = time(NULL);

    sqlite3_bind_int64(st, 1, (sqlite3_int64)now);
    sqlite3_bind_text(st, 2, reason ? reason : "unknown", -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, ip, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 4, mac, -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(st);
    sqlite3_finalize(st);

    return (rc == SQLITE_DONE) ? 0 : -1;
}

static int db_query_sessions(const char *sql, const char *param,
                             session_rec_t **out_arr, int *out_count)
{
    sqlite3_stmt *st = NULL;
    if (sqlite3_prepare_v2(g_db, sql, -1, &st, NULL) != SQLITE_OK)
        return -1;

    if (param)
        sqlite3_bind_text(st, 1, param, -1, SQLITE_TRANSIENT);

    int cap = 16, cnt = 0;
    session_rec_t *arr = malloc(sizeof(session_rec_t) * cap);
    if (!arr) {
        sqlite3_finalize(st);
        return -1;
    }

    while (1) {
        int rc = sqlite3_step(st);
        if (rc == SQLITE_ROW) {
            if (cnt >= cap) {
                cap *= 2;
                session_rec_t *tmp = realloc(arr, sizeof(session_rec_t) * cap);
                if (!tmp) {
                    free(arr);
                    sqlite3_finalize(st);
                    return -1;
                }
                arr = tmp;
            }
            session_rec_t *s = &arr[cnt++];
            memset(s, 0, sizeof(*s));

            s->id          = sqlite3_column_int(st, 0);
            snprintf(s->ip,   sizeof(s->ip),   "%s", sqlite3_column_text(st, 1));
            snprintf(s->mac,  sizeof(s->mac),  "%s", sqlite3_column_text(st, 2));
            snprintf(s->user, sizeof(s->user), "%s", sqlite3_column_text(st, 3));
            s->login_time  = (time_t)sqlite3_column_int64(st, 4);
            s->logout_time = (time_t)sqlite3_column_int64(st, 5);

            const unsigned char *reason = sqlite3_column_text(st, 6);
            if (reason)
                snprintf(s->reason, sizeof(s->reason), "%s", reason);
        } else if (rc == SQLITE_DONE) {
            break;
        } else {
            free(arr);
            sqlite3_finalize(st);
            return -1;
        }
    }

    sqlite3_finalize(st);

    *out_arr   = arr;
    *out_count = cnt;
    return 0;
}

int db_get_online(session_rec_t **out_arr, int *out_count)
{
    const char *sql =
        "SELECT id, ip, mac, user, login_time, "
        "IFNULL(logout_time,0), IFNULL(reason,'') "
        "FROM sessions WHERE logout_time IS NULL;";
    return db_query_sessions(sql, NULL, out_arr, out_count);
}

int db_get_online_for_restore(session_rec_t **out_arr, int *out_count)
{
    // 语义上强调“恢复”，实际与 get_online 相同
    return db_get_online(out_arr, out_count);
}

int db_get_history_by_user(const char *user, session_rec_t **out_arr, int *out_count)
{
    const char *sql =
        "SELECT id, ip, mac, user, login_time, "
        "IFNULL(logout_time,0), IFNULL(reason,'') "
        "FROM sessions WHERE user=? ORDER BY login_time DESC LIMIT 1000;";
    return db_query_sessions(sql, user, out_arr, out_count);
}