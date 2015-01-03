#ifndef __OP_H
#define __OP_H

/*
    MIME-types
*/

static inline char * mime_type(char *path) 
{
    int n = strlen(path);
    
    #define MIME(ext, type) if(!strcmp(&path[n - strlen(ext)], ext)) return type;

    MIME(".js",   "application/javascript")
    MIME(".css",  "text/css")
    MIME(".html", "text/html")

    MIME(".png",  "image/png")
    MIME(".jpg",  "image/jpeg")
    MIME(".jpeg", "image/jpeg")
    MIME(".gif",  "image/gif")
    MIME(".svg",  "image/svg+xml")

    MIME(".ico",  "image/x-icon")
    MIME(".eot",  "application/vnd.ms-fontobject")
    MIME(".ttf",  "application/font-ttf")
    MIME(".woff", "application/font-woff")
    
    return "text/plain";
}


/*
    Routing and states
*/

int callback_http(struct libwebsocket_context *,
                  struct libwebsocket *,
                  enum libwebsocket_callback_reasons, 
                  void *,
                  void *, 
                  size_t);

int callback_stat(struct libwebsocket_context *,
                  struct libwebsocket *,
                  enum libwebsocket_callback_reasons, 
                  void *,
                  void *, 
                  size_t);

/*
    Session data  
*/
#define MAX_STAT_REQ 128 
#define MAX_STAT_FILES 255

struct stat_session_data {
    char reqs[MAX_STAT_FILES][MAX_STAT_REQ];
    int fds[MAX_STAT_FILES];
    uint len;
};

static struct libwebsocket_protocols protocols[] = {
    { "http-only", callback_http, 0 },
    { "stat", callback_stat, sizeof(struct stat_session_data) },
    { NULL, NULL, 0 }
};

#endif