#include "http_manager.h"
#include "auth.h"
#include "boards.h"
#include <time.h>

static void ev_handler(struct mg_connection* c, int ev, void* ev_data);

uint8_t* jwt_secret;
sqlite3* db;
int start_http_server(sqlite3* db_param, uint8_t* secret){

    jwt_secret = secret;
    db = db_param;

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, "http://0.0.0.0:8000", ev_handler, NULL);
    for(;;){
        mg_mgr_poll(&mgr, 1000);
    }

    return 0;
}
int authenticated_routing(struct mg_conection* c, struct mg_http_message* hm, struct mg_str claim_buf);
static void ev_handler(struct mg_connection* c, int ev, void* ev_data){
    if(ev == MG_EV_HTTP_MSG){
        struct mg_http_message* hm = (struct mg_http_message*) ev_data;
        struct mg_str caps[5] = {0};

        int status_code = -1;

        if(mg_match(hm->uri, mg_str("/api/jwt"), caps)){
            char buf[128] = {0};
            mg_base64_encode(jwt_secret, SECRET_LENGTH, buf, sizeof(buf));
            mg_http_reply(c, 200, GLOBAL_HEADERS, "%s", buf);
        } else if(mg_match(hm->uri, mg_str("/api/auth/*"), caps)){
            status_code = auth_endpoint(c, hm, jwt_secret, caps[0]);
        } else if(mg_match(hm->uri, mg_str("/api/verify/*"), caps)){
            status_code = verify_endpoint(c, hm, jwt_secret, caps[0]);
        }

        if(mg_match(hm->uri, mg_str("/api#"), caps)){
            struct mg_str* auth = mg_http_get_header(hm, "Authorization");
            struct mg_str jwt_token[2] = {0};
            
            if(auth != NULL && mg_match(mg_str("Bearer *"), *auth, jwt_token)){
                char claim_buf[1024] = {0};

                struct mg_jwt_opts opts = {.secret = mg_str_n((char*)jwt_secret, SECRET_LENGTH)};
                size_t n = mg_jwt_verify_hs256(jwt_token[0], &opts, claim_buf, sizeof(claim_buf)-1);
                if(n == 0){
                    mg_http_reply(c, 401, GLOBAL_HEADERS NO_CACHE CONTENT_JSON, "{\"ok\":false,\"error\":\"Unauthorized. Invaild signature.\"}");
                    status_code = 401;
                    goto end;
                }

                long exp = mg_json_get_long(mg_str(claim_buf), "$.exp", 0);
                if(exp < time(NULL)){
                    mg_http_reply(c, 401, GLOBAL_HEADERS NO_CACHE CONTENT_JSON, "{\"ok\":false,\"error\":\"Unauthorized. Past experation.\"}");
                    status_code = 401;
                    goto end;
                }
                // more stuff

                status_code = authenticated_routing(c, hm, mg_str(claim_buf));
                if(status_code != -1) goto end;

                mg_http_reply(c, 404, GLOBAL_HEADERS HOUR_CACHE CONTENT_JSON, "{\"ok\":false,\"error\":\"Not found.\"}");
                status_code = 404;
            }
            mg_http_reply(c, 401, GLOBAL_HEADERS NO_CACHE CONTENT_JSON, "{\"ok\":false,\"error\":\"Unauthorized. Invaild or no Bearer token.\"}");
            status_code = 401;
            goto end;
        } else {
            struct mg_http_serve_opts opts = {.root_dir = FILE_PATH "public", .page404 = FILE_PATH "public/error404.txt", .enable_dir_listing = true};
            mg_http_serve_dir(c, hm, &opts);
        }

        end:

        if(status_code != -1) printf("stauts code: %d\n", status_code);
    }
}

int authenticated_routing(struct mg_conection* c, struct mg_http_message* hm, struct mg_str claim_buf){
    struct mg_str caps[5] = {0};
    bool is_admin = false;
    mg_json_get_bool(claim_buf, "$.admin", &is_admin);

    if(mg_match(hm->uri, mg_str("/api/board"), caps) && mg_strcasecmp(hm->method, mg_str("POST")) == 0){ // ADMIN endpoint: posting/creating a new board 
        if(!is_admin){
            mg_http_reply(c, 401, GLOBAL_HEADERS NO_CACHE CONTENT_JSON, "{\"ok\":false,\"error\":\"Unauthorized. This is an admin endpoint.\"}\n");
            return 401;
        }
        return post_board(c, db, hm->body);
    }
}
