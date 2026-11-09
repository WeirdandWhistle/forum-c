#include "auth.h"
#include <time.h>

int auth_endpoint(struct mg_connection* c, struct mg_http_message* hm, uint8_t* jwt_secret, struct mg_str type){
    struct mg_str secret = {.buf = (char *) jwt_secret, .len = SECRET_LENGTH};

    if(mg_strcmp(mg_str("anon"), type) == 0){
        unsigned long sub;
        mg_random(&sub, sizeof(sub));

        unsigned long exp = time(NULL) + JWT_EXP;
        unsigned long iat = time(NULL);

        char claim_buf[1024];
        mg_snprintf(claim_buf, sizeof(claim_buf), "{\"iss\":\"%s\",\"sub\":%lu,\"exp\":%lu,\"iat\":%lu,\"admin\":false}", JWT_ISS, sub, exp, iat);

        struct mg_jwt_opts opts = {.kid = mg_str(KEY_ID_HEADER), .secret = secret, .claims = mg_str(claim_buf)};
        char jwt_buf[4096] = {0};
        size_t n = mg_jwt_sign_hs256(&opts, jwt_buf, sizeof(jwt_buf));
        if(n == 0){
            mg_http_reply(c, 500, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"JWT signing faild\"}");
            return 500;
        }

        mg_http_reply(c, 200, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":true,\"data\":{\"jwt\":\"%s\"}}", jwt_buf);
        return 200;
    } else {
        mg_http_reply(c, 501, GLOBAL_HEADERS CONTENT_JSON NO_CACHE, "{\"ok\":false,\"error\":\"Not implemneted\"}");
        return 501;
    }
}
int verify_endpoint(struct mg_connection* c, struct mg_http_message* hm, uint8_t* secret, struct mg_str jwt){
    bool valid_token = verify_token(secret, jwt);
    mg_http_reply(c, 200, GLOBAL_HEADERS DAY_CACHE CONTENT_JSON, "{\"ok\":true,\"data\":{\"valid\":%s}}", valid_token ? "true" : "false");
    return 200;
}
bool verify_token(uint8_t* jwt_secret, struct mg_str jwt){
    struct mg_str secret = {.buf = (char*) jwt_secret, .len = SECRET_LENGTH};
    struct mg_jwt_opts opts = {.secret = secret};
    char buf[1024];
    size_t n = mg_jwt_verify_hs256(jwt, &opts, buf, sizeof(buf)-1);
    if(n == 0){
        return false;
    } else {
        long exp = mg_json_get_long(mg_str(buf), "$.exp", 0);
        if(exp < time(NULL)) return false;
        return true;
    }
}

int generate_admin_jwt(uint8_t* jwt_secret){
    unsigned long exp = time(NULL) + JWT_EXP;
    unsigned long iat = time(NULL);
    char claim_buf[1024];
    mg_snprintf(claim_buf, sizeof(claim_buf), "{\"iss\":\"%s\",\"sub\":\"admin\",\"exp\":%lu,\"iat\":%lu,\"admin\":true}", JWT_ISS, exp, iat);

    struct mg_str secret = {.buf = (char*) jwt_secret, .len = SECRET_LENGTH};
    struct mg_jwt_opts opts = {.kid = mg_str(KEY_ID_HEADER), .secret = secret, .claims = mg_str(claim_buf)};
    char jwt_buf[4096] = {0};
    size_t n = mg_jwt_sign_hs256(&opts, jwt_buf, sizeof(jwt_buf));
    if(n == 0){
        printf("Failed to sign a JWT for the admin.\n");
        return 1;
    }

    FILE* file = fopen(ADMIN_JWT_FILE, "w");
    if(!file){
        printf("Failed to open admin JWT file: %s\n", strerror(errno));
        return 1;
    }

    fwrite(jwt_buf, 1, n, file);

    fclose(file);
    return 0;
}