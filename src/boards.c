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
    if(rules == NULL){
        mg_http_reply(c, 400, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"rules are null.\"}\n");
        return 400;
    }
    // int rules_len = strlen(rules);

    char* note = mg_json_get_str(json, "$.note");
    if(note == NULL){
        mg_free(rules);
        mg_http_reply(c, 400, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"note are null.\"}\n");
        return 400;
    }


    // finish the SQLite call to post a board
    int64_t id;
    mg_random(&id, sizeof(id));
    id = llabs(id);

    static char post_board_sql[] = "INSERT INTO board (id, name, path, rules, note) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* post_board_stmt;
    rc = sqlite3_prepare_v2(db, post_board_sql, sizeof(post_board_sql), &post_board_stmt, NULL);
    if(rc != SQLITE_OK){
        mg_free(rules);
        mg_free(note);
        mg_http_reply(c, 500, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"SQLite failed to prepare stmt for posting: %M\"}\n", MG_ESC(sqlite3_errmsg(db)));
        return 500;
    }


    if(sqlite3_bind_int64(post_board_stmt, 1, id) != SQLITE_OK) goto clean_up_binding_posting;
    if(sqlite3_bind_text(post_board_stmt, 2, name, -1, NULL) != SQLITE_OK) goto clean_up_binding_posting;
    if(sqlite3_bind_text(post_board_stmt, 3, path + path_offset, -1, NULL) != SQLITE_OK) goto clean_up_binding_posting;
    if(sqlite3_bind_text(post_board_stmt, 4, rules, -1, NULL) != SQLITE_OK) goto clean_up_binding_posting;
    if(sqlite3_bind_text(post_board_stmt, 5, note, -1, NULL) != SQLITE_OK) goto clean_up_binding_posting;
    
    goto post_post_binding;
    clean_up_binding_posting:
    mg_http_reply(c, 500, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"SQLite failed to bind while posting: %M\"}\n", MG_ESC(sqlite3_errmsg(db)));
    mg_free(rules);
    mg_free(note);
    sqlite3_finalize(post_board_stmt);
    return 500;
    post_post_binding:

    rc = sqlite3_step(post_board_stmt);
    if(rc != SQLITE_DONE){
        rc = 500;
        mg_http_reply(c, rc, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"SQLite failed to step while posting: %M\"}\n", MG_ESC(sqlite3_errmsg(db)));
    } else {
        rc = 200;
        mg_http_reply(c, rc, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":true,\"data\":{\"id\":%ld}}\n", id);
    }

    sqlite3_finalize(post_board_stmt);
    mg_free(rules);
    mg_free(note);
    return rc;
}
int get_boards(struct mg_connection* c, sqlite3* db){
    static char get_boards_sql[] = "SELECT json_group_array(json_object('id', id, 'path', path, 'name', name, 'rules', rules, 'note', note)) FROM board;";
    sqlite3_stmt* get_boards_stmt;
    int rc = sqlite3_prepare_v2(db, get_boards_sql, sizeof(get_boards_sql), &get_boards_stmt, NULL);
    if(rc != SQLITE_OK){
        mg_http_reply(c, 500, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"SQLite error while preparing: %M\"}\n", MG_ESC(sqlite3_errmsg(db)));
    }

    rc = sqlite3_step(get_boards_stmt);
    if(rc != SQLITE_ROW){
        mg_http_reply(c, 500, GLOBAL_HEADERS CONTENT_JSON, NO_CACHE, "{\"ok\":false,\"error\":\"SQLite failed while getting row: %M\"}\n", MG_ESC(sqlite3_errmsg(db)));
        sqlite3_finalize(get_boards_stmt);
        return 500;
    }

    mg_http_reply(c, 200, GLOBAL_HEADERS CONTENT_JSON HOUR_CACHE, "{\"ok\":true,\"data\":%s}\n", sqlite3_column_text(get_boards_stmt, 0));

    sqlite3_finalize(get_boards_stmt);
    return 200;
}


int hot_load_boards_callback(void* void_ptr, int col_num, char** texts, char** names){
    struct mg_str** storage_ptr = ((struct mg_str**) void_ptr);

    struct mg_str* paths;
    int paths_len = 0;
    if(*storage_ptr == NULL){
        // printf("mallocing\n");
        paths = malloc(2 * sizeof(struct mg_str));
        paths_len = 1;
        if(!paths) return 1;
    } else {
        paths = *storage_ptr;
        while(paths[paths_len].buf != NULL){
            paths_len++;
        }
        paths_len += 1; // for new path to push on top
        // printf("reallocing with new paths_len of %d\n", paths_len);
        paths = realloc(paths, (paths_len + 1) * sizeof(struct mg_str));
    }   
     
    int path_len = strlen(texts[0]);
    paths[paths_len-1].len = path_len;
    paths[paths_len-1].buf = malloc(path_len+1);
    if(!paths[paths_len-1].buf){
        free(paths);
        return 1;
    }
    strcpy(paths[paths_len-1].buf, texts[0]);
    // printf("sqlite paths_len=%d: %s\n", paths_len, paths[paths_len-1].buf);

    paths[paths_len].len = 0;
    paths[paths_len].buf = NULL;

    *storage_ptr = paths;

    return 0;
}
int loaded_board_paths_len(struct mg_str* paths){
    int i = 0;
    while(paths[i].buf != NULL) i++;
    return i;
}
struct mg_str* hot_load_board_paths(sqlite3* db){

    struct mg_str* paths;

    static char get_boards_sql[] = "SELECT path FROM board;";
    int rc = sqlite3_exec(db, get_boards_sql, &hot_load_boards_callback, &paths, NULL);
    if(rc) printf("SQLite exec error: %s\n", sqlite3_errmsg(db));
    if(!paths) printf("Paths is still null.\n");
    else {
        int i = 0;
        while(paths[i].buf != NULL){
            
            printf("Loaded i=%d: '/%s'\n", i, paths[i].buf);
            i++;
        }
    }
    return rc == 0 ? paths : NULL;
}
void free_loaded_board_paths(struct mg_str* paths){
    int i = 0;
    while(paths[i].len != 0){
        free(paths[i].buf);
        i++;
    }
    free(paths);
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