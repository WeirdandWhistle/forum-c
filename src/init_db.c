#include "main.h"
#include "init_db.h"

int init_db(sqlite3* db){
    int out = 0;
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, DB_INIT_TABLE_BOARD, sizeof(DB_INIT_TABLE_BOARD), &stmt, NULL);
    if(rc != SQLITE_OK){
        printf("SQLite errored while trying to init tables: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    while((rc = sqlite3_step(stmt)) != SQLITE_DONE){
        if(rc == SQLITE_ERROR){
            printf("SQLite errored while trying to step: %s\n", sqlite3_errmsg(db));
            out = 1;
            break;
        }
    }
    sqlite3_finalize(stmt);
    if(out) return out;

    rc = sqlite3_prepare_v2(db, DB_INIT_TABLE_POST, sizeof(DB_INIT_TABLE_POST), &stmt, NULL);
    if(rc != SQLITE_OK){
        printf("SQLite errored while trying to init tables: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    while((rc = sqlite3_step(stmt)) != SQLITE_DONE){
        if(rc == SQLITE_ERROR){
            printf("SQLite errored while trying to step: %s\n", sqlite3_errmsg(db));
            out = 1;
            break;
        }
    }
    sqlite3_finalize(stmt);
    if(out) return out;

    rc = sqlite3_prepare_v2(db, DB_INIT_TABLE_COMMNET, sizeof(DB_INIT_TABLE_COMMNET), &stmt, NULL);
    if(rc != SQLITE_OK){
        printf("SQLite errored while trying to init tables: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    while((rc = sqlite3_step(stmt)) != SQLITE_DONE){
        if(rc == SQLITE_ERROR){
            printf("SQLite errored while trying to step: %s\n", sqlite3_errmsg(db));
            out = 1;
            break;
        }
    }
    sqlite3_finalize(stmt);
    return out;
}