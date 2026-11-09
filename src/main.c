#include "main.h"
#include "init_db.h"
#include "http_manager.h"
#include "auth.h"

int load_secret(uint8_t* secret);
int main(int argc, char* argv[]){
    uint8_t secret[SECRET_LENGTH] = {0};
    int rc = load_secret(secret);
    if(rc){
        return 1;
    }

    rc = generate_admin_jwt(secret);
    if(rc){
        return 1;
    }

    sqlite3* db;
    rc = sqlite3_open(DATABASE_FILE, &db);
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

    rc = start_http_server(db, secret);

    if(!rc) printf("Successfuly and safely exiting.\n");
    sqlite3_close(db);

    return rc;
}

int load_secret(uint8_t* secret){
    if(access(SECRET_FILE, F_OK) == 0){
        FILE* file = fopen(SECRET_FILE, "r");
        if(!file){
            printf("Could NOT open and read secret file: %s\n", strerror(errno));
            return 1;
        }

        fread(secret, sizeof(uint8_t), SECRET_LENGTH, file);

        fclose(file);
    } else {
        mg_random(secret, SECRET_LENGTH);

        FILE* file = fopen(SECRET_FILE, "w");
        if(!file){
            printf("Could NOT open and write secret file: %s\n", strerror(errno));
            return 1;
        }

        fwrite(secret, sizeof(uint8_t), SECRET_LENGTH, file);

        fclose(file);
    }
    return 0;
}