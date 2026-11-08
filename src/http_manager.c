#include "http_manager.h"
#include "auth.h"

static void ev_handler(struct mg_connection* c, int ev, void* ev_data);

uint8_t* jwt_secret;
int start_http_server(sqlite3* db, uint8_t* secret){

    jwt_secret = secret;

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
        struct mg_str caps[16];

        int status_code = -1;

        if(mg_match(hm->uri, mg_str("/api/jwt"), caps)){
            char buf[128];
            mg_base64_encode(jwt_secret, SECRET_LENGTH, buf, sizeof(buf));
            mg_http_reply(c, 200, GLOBAL_HEADERS, "%s", buf);
        } else if(mg_match(hm->uri, mg_str("/api/auth/*"), caps)){
            status_code = auth_endpoint(c, hm, jwt_secret, caps[0]);
        }

        struct mg_http_serve_opts opts = {.root_dir = FILE_PATH "public", .page404 = FILE_PATH "public/error404.txt", .enable_dir_listing = true};
        mg_http_serve_dir(c, hm, &opts);

        if(status_code != -1) printf("stauts code: %d\n", status_code);
    }
}
