#include <unistd.h>
#include <err.h>
#include <stdlib.h>

#include "misc.h"
#include "outstream.h"

void out_stream_init(struct out_stream *s, char *proc_name) {
    s->buf_len = 0;
    s->proc_name = proc_name;
}

void out_stream_write_char(struct out_stream *s, char c) {
    if(s->buf_len >= BUFFER_SIZE) {
        out_stream_flush(s);
    }

    s->buf[s->buf_len++] = c;
}

void out_stream_write_str(struct out_stream *s, char *str) {
    for(char *c = str; *c != '\0'; c++) {
        out_stream_write_char(s, *c);
    }
}

void out_stream_flush(struct out_stream *s) {
    int written = 0;

    while(written < s->buf_len) {
        int ret = write(STDOUT_FILENO, s->buf + written, s->buf_len - written);

        if(ret == -1) {
            err(EXIT_FAILURE, "can't write the output");
        }

        written += ret;
    }

    s->buf_len = 0;
}
