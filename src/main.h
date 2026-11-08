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
    #define SECRET_LENGTH 32


    #define GLOBAL_HEADERS ""

#endif