#ifndef __CONFIG_H
#define __CONFIG_H

/*
    Web-server configuratons
*/

#define MAX_STATIC_PATH 64
#define DOCUMENT_ROOT "/home/mkrentovskiy/develop/nets/showmestat/wwwroot"
#define STATIC_PATH_LEN (strlen(DOCUMENT_ROOT) + MAX_STATIC_PATH)

static struct lws_context_creation_info webserver_config = {
            .port = 8000,
            .iface = NULL,
            .protocols = protocols, /* -> op.h */
            .extensions = NULL,
            .token_limits = NULL,
            .ssl_cert_filepath = NULL,
            .ssl_private_key_filepath = NULL,
            .ssl_ca_filepath = NULL,
            .ssl_cipher_list = NULL,
            .gid = -1,
            .uid = -1,
            .options = 0,
            .user = NULL,
            .ka_time = 180,
            .ka_probes = 3,
            .ka_interval = 500,
            .provided_client_ssl_ctx = NULL
        };

/*
    Statdata configuration
*/

#define DATA_PREFIX "/home/mkrentovskiy/develop/nets/showmestat/mocks"
#define DATA_PATH_LEN (strlen(DATA_PREFIX) + MAX_STATIC_PATH)
#define MAX_BUFFER_LEN 64 * 1024 // 64k

#endif