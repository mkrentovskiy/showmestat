#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libwebsockets.h>

/*
    MIME-types
*/

#define MIME(ext, type) if(strcmp(extension, ext) == 0) return type;

static inline char * mime_type(char *resource_path) 
{
    char *extension = strrchr(resource_path, '.');

    if(extension == NULL) return "text/plain";
    MIME(".png", "image/png")
    MIME(".jpg", "image/jpg")
    MIME(".css", "image/css")
    MIME(".html", "text/html")
    MIME(".svg", "image/svg")

    return "text/plain";
}


/*
    Routing
*/

int callback_http(struct libwebsocket_context *context,
                         struct libwebsocket *wsi,
                         enum libwebsocket_callback_reasons reason, void *user,
                         void *in, size_t len);
int callback_stat(struct libwebsocket_context *context,
                         struct libwebsocket *wsi,
                         enum libwebsocket_callback_reasons reason, void *user,
                         void *in, size_t len);

static struct libwebsocket_protocols protocols[] = {
    { "http-only", callback_http, 0 },
    { "stat", callback_stat, 0 },
    { NULL, NULL, 0 }
    
};

/*
    Web-server configuratons
*/

#define DOCUMENT_ROOT "/home/mkrentovskiy/develop/nets/showmestat/wwwroot"
#define MAX_STATIC_PATH 30
#define STATIC_PATH_LEN (strlen(DOCUMENT_ROOT) + MAX_STATIC_PATH)

static struct lws_context_creation_info webserver_config = {
            .port = 8000,
            .iface = NULL,
            .protocols = protocols,
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


#endif