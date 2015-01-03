#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <libwebsockets.h>

#include "op.h"
#include "config.h"


static inline void stat_open(struct stat_session_data *state) 
{

}

static inline void stat_process(struct stat_session_data *state, char *req)
{}


static inline void stat_close(struct stat_session_data *state) 
{

}



int callback_stat(struct libwebsocket_context *context,
                  struct libwebsocket *wsi,
                  enum libwebsocket_callback_reasons reason, 
                  void *user,
                  void *in, 
                  size_t len)
{
    struct stat_session_data *state = (struct stat_session_data *) user;

    switch(reason) {
        case LWS_CALLBACK_CLIENT_WRITEABLE: {
            stat_open(state); 
            break;            
        }
            
        case LWS_CALLBACK_HTTP: {
            char *request = (char *) in;
            
            stat_process(state, request);
            break;
        }

        case LWS_CALLBACK_CLOSED_HTTP:
        {
            stat_close(state); 
            break;            
        }
    }
    return 0;
}
