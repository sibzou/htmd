#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>

#include "misc.h"
#include "instream.h"

void in_stream_init(struct in_stream *s) {
    s->buf_len = 0;
    s->read_head = 0;
}

bool in_stream_read(struct in_stream *s, char *c) {
    if(s->read_head >= s->buf_len) {
        s->buf_len = read(STDIN_FILENO, s->buf, BUFFER_SIZE);

        if(s->buf_len == -1) {
            err(EXIT_FAILURE, "can't read the input");
        } else if(s->buf_len == 0) {
            return true;
        }

        s->read_head = 0;
    }

    *c = s->buf[s->read_head];
    s->read_head++;
    return false;
}
