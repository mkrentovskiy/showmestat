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


static inline bool validate(char *in) 
{
    while(*in != '\0') {
        if(!((*in >= '0' && *in <= '9') || (*in >= 'a' && *in <= 'z')) && *in != '.')
            return false; /* [0-9a-z\.]* */
        in++;
    }
    return true;
}

static inline int req_to_buf(struct stat_session_data *state,
                             char *in, 
                             char *buf, 
                             uint max_len) 
{
    return 0;
}

int callback_stat(struct libwebsocket_context *context,
                  struct libwebsocket *wsi,
                  enum libwebsocket_callback_reasons reason, 
                  void *user,
                  void *in, 
                  size_t len)
{
    struct stat_session_data *state = (struct stat_session_data *) user;
    char buf[LWS_SEND_BUFFER_PRE_PADDING + MAX_BUFFER_LEN + LWS_SEND_BUFFER_POST_PADDING];
    uint buflen = 0;

    switch(reason) {
        case LWS_CALLBACK_ESTABLISHED: {
            lwsl_notice("STAT: Connection established\n");
            break;
        }
            
        case LWS_CALLBACK_RECEIVE: { 
            if(!validate((char *) in)) break;

            memset(&buf[LWS_SEND_BUFFER_PRE_PADDING], 0, MAX_BUFFER_LEN);            
            sprintf(&buf[LWS_SEND_BUFFER_PRE_PADDING], 
                    "{\"req\": \"%s\", \"reply\": \"", 
                    (char *) in);
            buflen = strlen(&buf[LWS_SEND_BUFFER_PRE_PADDING]);
            buflen += req_to_buf(state,
                                 (char *)in, 
                                 &buf[LWS_SEND_BUFFER_PRE_PADDING + buflen], 
                                 MAX_BUFFER_LEN - buflen - 3); /* "}/0 */

            buf[LWS_SEND_BUFFER_PRE_PADDING + buflen] = '"'; 
            buf[LWS_SEND_BUFFER_PRE_PADDING + buflen + 1] = '}'; 
            buf[LWS_SEND_BUFFER_PRE_PADDING + buflen + 2] = '/0'; 
            buflen += 2; 

            libwebsocket_write(wsi, &buf[LWS_SEND_BUFFER_PRE_PADDING], buflen, LWS_WRITE_TEXT);
            break;
        }
    }
    return 0;
}
