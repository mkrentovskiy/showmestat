#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libwebsockets.h>

#include "config.h"

int callback_http(struct libwebsocket_context *context,
                         struct libwebsocket *wsi,
                         enum libwebsocket_callback_reasons reason, void *user,
                         void *in, size_t len)
{
    
    switch (reason) {
        case LWS_CALLBACK_CLIENT_WRITEABLE:
            printf("connection established\n");
            
        case LWS_CALLBACK_HTTP: {
            char *requested_uri = (char *) in;
            char resource_path[STATIC_PATH_LEN];
            
            printf("requested URI: %s\n", requested_uri);
            if(strcmp(requested_uri, "/") == 0) requested_uri = "/index.html";

            if(strlen(requested_uri) < MAX_STATIC_PATH) {                   
                sprintf(resource_path, DOCUMENT_ROOT"%s", requested_uri);
                printf("resource path: %s\n", resource_path);
                
                char *extension = strrchr(resource_path, '.');
                char *mime;
                
                if(extension == NULL) {
                    mime = "text/plain";
                } else if (strcmp(extension, ".png") == 0) {
                    mime = "image/png";
                } else if (strcmp(extension, ".jpg") == 0) {
                    mime = "image/jpg";
                } else if (strcmp(extension, ".gif") == 0) {
                    mime = "image/gif";
                } else if (strcmp(extension, ".html") == 0) {
                    mime = "text/html";
                } else if (strcmp(extension, ".css") == 0) {
                    mime = "text/css";
                } else {
                    mime = "text/plain";
                }

                libwebsockets_serve_http_file(context, wsi, resource_path, mime, NULL, 0);
            }
            break;
        }
    }
    return 0;
}

int callback_stat(struct libwebsocket_context *context,
                         struct libwebsocket *wsi,
                         enum libwebsocket_callback_reasons reason, void *user,
                         void *in, size_t len)
{
    
    switch (reason) {
        case LWS_CALLBACK_CLIENT_WRITEABLE:
            printf("connection established\n");
            
        case LWS_CALLBACK_HTTP: {
            char *requested_uri = (char *) in;
            printf("requested URI: %s\n", requested_uri);
            
            if (strcmp(requested_uri, "/") == 0) {
                void *universal_response = "Hello, World!";
                libwebsocket_write(wsi, universal_response,
                                   strlen(universal_response), LWS_WRITE_HTTP);
                break;

            } else {
                char resource_path[STATIC_PATH_LEN];
                
                if(strlen(requested_uri) < MAX_STATIC_PATH) {                   
                    sprintf(resource_path, DOCUMENT_ROOT"%s", requested_uri);
                    printf("resource path: %s\n", resource_path);
                    
                    char *extension = strrchr(resource_path, '.');
                    char *mime;
                    
                    if(extension == NULL) {
                        mime = "text/plain";
                    } else if (strcmp(extension, ".png") == 0) {
                        mime = "image/png";
                    } else if (strcmp(extension, ".jpg") == 0) {
                        mime = "image/jpg";
                    } else if (strcmp(extension, ".gif") == 0) {
                        mime = "image/gif";
                    } else if (strcmp(extension, ".html") == 0) {
                        mime = "text/html";
                    } else if (strcmp(extension, ".css") == 0) {
                        mime = "text/css";
                    } else {
                        mime = "text/plain";
                    }

                    libwebsockets_serve_http_file(context, wsi, resource_path, mime, NULL, 0);
                }
            }
            break;
        }
    }
    return 0;
}

int main(void) {
    struct libwebsocket_context *context;

    context = libwebsocket_create_context(&webserver_config);
    
    if(context == NULL) {
        lwsl_err("libwebsocket init failed");
        return -1;
    }
    
    lwsl_notice("starting server");
    while(1) libwebsocket_service(context, 50); 
    lwsl_notice("stoping server");
    libwebsocket_context_destroy(context);
    
    return 0;
}
