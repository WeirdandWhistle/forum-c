#include "main.h"
#include "init_db.h"

int init_db(sqlite3* db){

    if(sqlite3_exec(db, DB_INIT_TABLES, NULL, NULL, NULL)){
        printf("SQLite error on initing table board: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    #ifdef ENABLE_DEFAULT_BOARDS

    sqlite3_exec(db, "INSERT INTO board (id, path, name, note, rules) VALUES (1 , 'general', 'General', 'A little bit of everying', 'Dont keep it too specific.');", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO board (id, path, name, note, rules) VALUES (2 , 'linux', 'Linux', 'Talk about linux', 'Only thing relating to linux.');", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO board (id, path, name, note, rules) VALUES (3 , 'c', 'Programming', 'Talk about the best programming language ;)', 'Any thing related to programming.');", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO board (id, path, name, note, rules) VALUES (4 , 'news', 'News', 'Whats happening in the news?', 'Talk about the happenings of the world.');", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO board (id, path, name, note, rules) VALUES (5 , 'politics', 'Politics', 'Wanna get mad?', 'Talk, argue, and debate.');", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO board (id, path, name, note, rules) VALUES (6 , 'qna', 'Q & A', 'People talk about issues they think other people car about (they dont)', 'Create an ordered list of questions and answers.');", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO board (id, path, name, note, rules) VALUES (7 , 'games', 'Gaming', 'All things gaming', 'Any thing gaming related.');", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO board (id, path, name, note, rules) VALUES (8 , 'food', 'Food', 'The stuff you need to eat to live', 'food.');", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO board (id, path, name, note, rules) VALUES (9 , 'film', 'Films', 'Talk about your favorite Films', 'Movies / Films');", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO board (id, path, name, note, rules) VALUES (10, 'shows', 'Shows', 'Talk about your favorite TV Shows', 'TV Shows');", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO board (id, path, name, note, rules) VALUES (12, 'book', 'Books', 'Talk about your favorite Books', 'Book, eBooks, and audio-books. idc');", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO board (id, path, name, note, rules) VALUES (13, 'ram', 'PC Building', 'Its dead', 'In this economy?!');", NULL, NULL, NULL);
    sqlite3_exec(db, "INSERT INTO board (id, path, name, note, rules) VALUES (14 , 'gta6', 'GTA 6', 'Its really coming out?!?!?!?', 'YAY!');", NULL, NULL, NULL);
    // sqlite3_exec(db, "INSERT INTO board (id, path, name, note, rules) VALUES ( , '', '', '', '');", NULL, NULL, NULL);

    #endif

    return 0;
}