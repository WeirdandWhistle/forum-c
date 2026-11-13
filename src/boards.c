#include "boards.h"

int post_board(struct mg_connection* c, sqlite3* db, struct mg_str json){

    char* path_buf = mg_json_get_str(json, "$.path");
    if(path_buf == NULL){
        mg_http_reply(c, 400, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"Path if missing or malfored.\"}\n");
        return 400;
    }
    size_t path_buf_len = strlen(path_buf);
    char path[128] = {0};
    memcpy(path, path_buf, path_buf_len > sizeof(path) - 2 ? sizeof(path) : path_buf_len);
    path[sizeof(path)-1] = 0;
    mg_free(path_buf);

    struct mg_str caps[2] = {0};
    if(!mg_match(mg_str(path), mg_str("/*"), caps)){
        mg_http_reply(c, 400, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"Path is malformed.\"}\n");
        return 400;
    }

    if(!is_all_digit_or_low_case_mg(caps[0])){
        mg_http_reply(c, 400, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"Path is malformed. lower case letters and digits after a / only.\"}\n");
        return 400;
    }
    // MG_PRINT_STR(caps[0]);
    // memcpy(path, caps[0].buf, caps[0].len);
    int path_offset = caps[0].buf - path;
    if(is_reserved(path + path_offset)){
        mg_http_reply(c, 400, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"That path is already resvered.\"}\n");
        return 400;
    }

    char* name_buf = mg_json_get_str(json, "$.name");
    if(!name_buf){
        mg_http_reply(c, 400, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"Name if missing or malfored.\"}\n");
        return 400;
    }
    size_t name_buf_len = strlen(name_buf);
    char name[128] = {0};
    memcpy(name, name_buf, name_buf_len > sizeof(name) - 1 ? sizeof(name)-1 : name_buf_len);
    mg_free(name_buf);

    static char check_path_and_name_sql[] = "SELECT id FROM board WHERE name = ? OR path = ? LIMIT 1;";
    sqlite3_stmt* check_path_and_name_stmt; 
    int rc = sqlite3_prepare_v2(db, check_path_and_name_sql, sizeof(check_path_and_name_sql), &check_path_and_name_stmt, NULL);
    if(rc != SQLITE_OK){
        mg_http_reply(c, 500, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"SQLite failed to prepare stmt: %M\"}\n", MG_ESC(sqlite3_errmsg(db)));
        return 500;
    }

    if(sqlite3_bind_text(check_path_and_name_stmt, 1, name, -1, NULL) != SQLITE_OK) goto clean_up_sqlite_binding; 
    if(sqlite3_bind_text(check_path_and_name_stmt, 2, path+ path_offset, -1, NULL) != SQLITE_OK) goto clean_up_sqlite_binding;

    goto sqlite_is_fine;
    clean_up_sqlite_binding:
    sqlite3_finalize(check_path_and_name_stmt);
    mg_http_reply(c, 500, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"SQLite failed to bind to stmt: %M\"}\n", MG_ESC(sqlite3_errmsg(db)));
    return 500;
    sqlite_is_fine:

    rc = sqlite3_step(check_path_and_name_stmt);
    if(rc == SQLITE_DONE){
        // everything is fine. no results tharts good
    } else if(rc == SQLITE_ROW){
        sqlite3_finalize(check_path_and_name_stmt);
        mg_http_reply(c, 400, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"Name or Path already in use.\"}\n");
        return 400;
    } else {
        sqlite3_finalize(check_path_and_name_stmt);
        mg_http_reply(c, 500, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"SQLite failed to step: %M\"}\n", MG_ESC(sqlite3_errmsg(db)));
        return 500;
    }

    sqlite3_finalize(check_path_and_name_stmt);

    char* rules = mg_json_get_str(json, "$.rules");
    // int rules_len = strlen(rules);

    // finish the SQLite call to post a board
    int64_t id;
    mg_random(&id, sizeof(id));
    id = llabs(id);

    static char post_board_sql[] = "INSERT INTO board (id, name, path, rules) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* post_board_stmt;
    rc = sqlite3_prepare_v2(db, post_board_sql, sizeof(post_board_sql), &post_board_stmt, NULL);
    if(rc != SQLITE_OK){
        mg_free(rules);
        mg_http_reply(c, 500, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"SQLite failed to prepare stmt for posting: %M\"}\n", MG_ESC(sqlite3_errmsg(db)));
        return 500;
    }


    if(sqlite3_bind_int64(post_board_stmt, 1, id) != SQLITE_OK) goto clean_up_binding_posting;
    if(sqlite3_bind_text(post_board_stmt, 2, name, -1, NULL) != SQLITE_OK) goto clean_up_binding_posting;
    if(sqlite3_bind_text(post_board_stmt, 3, path + path_offset, -1, NULL) != SQLITE_OK) goto clean_up_binding_posting;
    if(sqlite3_bind_text(post_board_stmt, 4, rules, -1, NULL) != SQLITE_OK) goto clean_up_binding_posting;
    
    goto post_post_binding;
    clean_up_binding_posting:
    mg_http_reply(c, 500, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"SQLite failed to bind while posting: %M\"}\n", MG_ESC(sqlite3_errmsg(db)));
    mg_free(rules);
    sqlite3_finalize(post_board_stmt);
    return 500;
    post_post_binding:

    rc = sqlite3_step(post_board_stmt);
    if(rc != SQLITE_DONE){
        rc = 500;
        mg_http_reply(c, rc, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"SQLite failed to step while posting: %M\"}\n", MG_ESC(sqlite3_errmsg(db)));
    } else {
        rc = 200;
        mg_http_reply(c, rc, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":true,\"data\":{}}\n");
    }

    sqlite3_finalize(post_board_stmt);
    mg_free(rules);
    return rc;
}
int get_boards(struct mg_connection* c, sqlite3* db){
    // static char[] = get_boards_sql = "SELECT id, path, name, rules FROM board;";
    // sqlite3_stmt* get_boards_stmt;
    // int rc = sqlite3_prepare_v2(db, get_boards_sql, sizeof(get_boards_sql), &get_boards_stmt, NULL);
    // if(rc != SQLITE_OK){
    //     mg_http_reply(c, 500, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\"\"\"}")
    // }

    mg_http_reply(c, 200, "", "{\"data\":\"error: %M\"}\n", MG_ESC("\"\" '' stuff super hard to escape and suck \""));
    return 200;
}

bool is_all_digit_or_low_case(char* str){
    int len = strlen(str);
    for(int i = 0; i < len; i++){
        if(!((48 <= str[i] && str[i] <= 57) && (97 <= str[i] && str[i] <= 122))) return false;
    }
    return true;
}
bool is_all_digit_or_low_case_mg(struct mg_str str){
    for(size_t i = 0; i < str.len; i++){
        if(!(
            (48 <= str.buf[i] && str.buf[i] <= 57) || 
            (97 <= str.buf[i] && str.buf[i] <= 122)
        )) return false;
    }
    return true;
}
bool is_reserved(char* str){
    static char* reserved_paths[] = RESERVED_PATHS;

    int len = ARRAY_LEN(reserved_paths);
    for(int i = 0; i < len; i++){
        if(strcmp(str, reserved_paths[i]) == 0) return true;
    }
    return false;
}