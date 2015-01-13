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


static inline bool validate(char *in) 
{
    while(*in != '\0') {
        if(!((*in >= '0' && *in <= '9') || (*in >= 'a' && *in <= 'z')) && *in != '.')
            return false; /* [0-9a-z\.]* */
        in++;
    }
    return true;
}

static inline int dump(int fd, 
                       char *buf, 
                       uint max_len)
{
    int i, r = 0;

    lseek(fd, 0, SEEK_SET);
    r = read(fd, buf, max_len);
    for(i = 0; i < r; i++) {
        if(buf[i] == 0x0a) buf[i] = ';';
    }
    return r;
} 


static inline int req_to_buf(struct stat_session_data *state,
                             char *in, 
                             char *buf, 
                             uint max_len) 
{
    int an = state->len++, i, fd, len = 0;
    char path[DATA_PATH_LEN];

    for(i = 0; i < state->len - 1; i++) {
        if(!strcmp(state->reqs[i], in)) { 
            state->len--;
            return dump(state->fds[i], buf, max_len);  
        } 
    }

#define ER(str) { lwsl_notice("STAT: "str" (%s)\n", in); return 0; }

    if(an >= MAX_STAT_FILES) ER("Nowere hide");
    if((len = strlen(in)) >= MAX_STAT_REQ) ER("Filename is too long");
    
    strcpy(path, DATA_PREFIX);
    strcpy(path + strlen(DATA_PREFIX), in);

    if( (fd = open(path, O_RDONLY)) > 0) {
        strcpy(state->reqs[an], in);
        state->fds[an] = fd;
        lwsl_notice("STAT: Add file %s \n", path);
        return dump(fd, buf, max_len);
    } else ER("Can't open file");  
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
                                 MAX_BUFFER_LEN - buflen - 3); /* "}\0 */

            buf[LWS_SEND_BUFFER_PRE_PADDING + buflen] = '"'; 
            buf[LWS_SEND_BUFFER_PRE_PADDING + buflen + 1] = '}'; 
            buf[LWS_SEND_BUFFER_PRE_PADDING + buflen + 2] = '\0'; 
            buflen += 2; 

            libwebsocket_write(wsi, (unsigned char *) &buf[LWS_SEND_BUFFER_PRE_PADDING], buflen, LWS_WRITE_TEXT);
            break;
        }

        default: {
            break;    
        }
    }
    return 0;
}
