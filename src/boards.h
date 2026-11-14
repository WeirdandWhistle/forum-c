#ifndef BOARDS
#define BOARDS

    #include "main.h"

    int post_board(struct mg_connection* c, sqlite3* db, struct mg_str json);
    int get_boards(struct mg_connection* c, sqlite3* db);

    struct mg_str* hot_load_board_paths(sqlite3* db);
    void free_loaded_board_paths(struct mg_str* paths);
    int loaded_board_paths_len(struct mg_str* paths);

    bool is_all_digit_or_low_case(char* str);
    bool is_all_digit_or_low_case_mg(struct mg_str str);
    bool is_reserved(char* str);

#endif