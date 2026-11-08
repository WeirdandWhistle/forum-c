#include "http_manager.h"

static void ev_handler(struct mg_connection* c, int ev, void* ev_data);

int start_http_server(sqlite3* db){

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, "http://0.0.0.0:8000", ev_handler, NULL);
    for(;;){
        mg_mgr_poll(&mgr, 1000);
    }

    return 0;
}

static void ev_handler(struct mg_connection* c, int ev, void* ev_data){
    if(ev == MG_EV_HTTP_MSG){
        struct mg_http_message* hm = (struct mg_http_message*) ev_data;

        struct mg_http_serve_opts* opts = {};

        mg_http_serve_file(c, hm, "public/index.html", opts);
    }
}
