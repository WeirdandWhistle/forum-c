#include "main.h"
#include "init_db.h"
#include "http_manager.h"

int main(int argc, char* argv[]){
    sqlite3* db;
    int rc = sqlite3_open(DATABASE_FILE, &db);
    if(rc != SQLITE_OK){
        printf("Failed to open database file: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    printf("Database open succefuly!\n");

    rc = init_db(db);
    if(rc){
        printf("Initialzing database tables failed.\n");
        sqlite3_close(db);
        return 1;
    }

    rc = start_http_server(db);

    if(!rc) printf("Successfuly and safely exiting.\n");
    sqlite3_close(db);

    return rc;
}