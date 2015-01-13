#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>

#include <libwebsockets.h>

#include "op.h"
#include "config.h"

int callback_http(struct libwebsocket_context *context,
                  struct libwebsocket *wsi,
                  enum libwebsocket_callback_reasons reason, 
                  void *user,
                  void *in, 
                  size_t len)
{
    switch(reason) {
        case LWS_CALLBACK_HTTP: {
            char *request = (char *) in;
            char resource_path[STATIC_PATH_LEN];
            
            lwsl_notice("HTTP: Requested URI: %s\n", request);
            if(!strcmp(request, "/")) request = "/index.html";
            if(strlen(request) < MAX_STATIC_PATH) {                   
                sprintf(resource_path, DOCUMENT_ROOT"%s", request);
                char *mime = mime_type(resource_path);

                lwsl_notice("HTTP: Resource path: %s, MIME: %s\n", resource_path, mime);
                libwebsockets_serve_http_file(context, wsi, resource_path, mime, NULL, 0);
            }
            break;
        }
        default: {
            break;    
        }

    }

    return 0;
}

bool internal_loop;
void sighandler(int sig)
{
    internal_loop = false;
}

int main(void) {
    struct libwebsocket_context *context;

    context = libwebsocket_create_context(&webserver_config);
    if(context == NULL) {
        lwsl_err("libwebsocket init failed.\n");
        return -1;
    }
    
    lwsl_notice("starting server.\n");
    internal_loop = true;
    signal(SIGINT, sighandler);

    while(internal_loop) libwebsocket_service(context, 50); 
    
    lwsl_notice("stoping server.\n");
    libwebsocket_context_destroy(context);
    
    return 0;
}
