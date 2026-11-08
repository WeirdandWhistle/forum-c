#ifndef INIT_DB
#define INIT_DB

    #define DB_INIT_TABLE_BOARD   "CREATE TABLE IF NOT EXISTS board   (id BIGINT PRIMARY KEY NOT NULL, name TEXT NOT NULL, path TEXT NOT NULL, rules TEXT);" 
    #define DB_INIT_TABLE_POST    "CREATE TABLE IF NOT EXISTS post    (id TEXT PRIMARY KEY NOT NULL, board_id BIGINT NOT NULL, title TEXT NOT NULL, content TEXT NOT NULL, created BIGINT NOT NULL, likes INT NOT NULL, dislikes INT NOT NULL, FOREIGN KEY (board_id) REFERENCES board(id));" 
    #define DB_INIT_TABLE_COMMNET "CREATE TABLE IF NOT EXISTS comment (id TEXT PRIMARY KEY NOT NULL, post_id TEXT, parrent_id TEXT, content TEXT NOT NULL, created BIGINT NOT NULL, FOREIGN KEY(post_id) REFERENCES post(id));"

    int init_db(sqlite3* db);

#endif