#include "main.h"
#include "init_db.h"

int init_db(sqlite3* db){

    if(sqlite3_exec(db, DB_INIT_TABLES, NULL, NULL, NULL)){
        printf("SQLite error on initing table board: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    return 0;
}