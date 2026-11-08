#ifndef AUTH
#define AUTH

    #include "main.h"

    int auth_endpoint(struct mg_connection* c, struct mg_http_message* hm, uint8_t* secret, struct mg_str type);

#endif