#ifndef MAIN
#define MAIN

    #include <stdio.h>
    #include <stdlib.h>
    #include <errno.h>
    #include "../lib/mongoose.h"
    #include "../lib/sqlite3.h" 

    #define FILE_PATH "./"

    #define DATABASE_FILE FILE_PATH "sql.db"
    #define SECRET_FILE FILE_PATH "jwt_secret"
    #define ADMIN_JWT_FILE FILE_PATH "admin_jwt.txt"
    #define SECRET_LENGTH 32
    #define KEY_ID_HEADER "default"
    #define JWT_EXP 60 * 60 * 24 * 90
    #define JWT_ISS "forum.whynotjava.net"



    #define NO_CACHE "Cache-Control: no-cache, max-age=0\r\n"
    #define HOUR_CACHE "Cache-Control: public, max-age=3600\r\n"
    #define DAY_CACHE "Cache-Control: public, max-age=86400\r\n"
    #define CONTENT_JSON "Content-Type: application/json\r\n"
    #define CONTENT_TEXT "Content-Type: application/text\r\n"
    #define GLOBAL_HEADERS ""


    #define RESERVED_PATHS {"posts", "post", "user", "users", "dm", "create", "delete", "message", "messages", "rss", "api", "authorize", "token", "cdn", "admin", "scripts", "src"}


    #define ARRAY_LEN(arr) sizeof(arr) / sizeof(arr[0])


    #define MG_PRINT_STR(str) for(size_t iiiiiiiiii  = 0; iiiiiiiiii < str.len; iiiiiiiiii++){printf("%c",str.buf[iiiiiiiiii]);} printf("\n");
#endif