#ifndef AUTH
#define AUTH

    #include "main.h"

    int auth_endpoint(struct mg_connection* c, struct mg_http_message* hm, uint8_t* secret, struct mg_str type);
    int verify_endpoint(struct mg_connection* c, struct mg_http_message* hm, uint8_t* secret, struct mg_str jwt);
    bool verify_token(uint8_t* secret, struct mg_str jwt);
    int generate_admin_jwt(uint8_t* jwt_secret);


#endif