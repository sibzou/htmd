#include <stdbool.h>

#include "htmd.h"
#include "misc.h"
#include "outstream.h"
#include "paragraph.h"

void paragraph_parser_init(struct paragraph_parser *s, struct out_stream *out_stream) {
    s->out_stream = out_stream;
    s->out_of_word = 3;
}

void paragraph_parse(struct paragraph_parser *s, struct parser_char *pch) {
    if(pch->end) {
        paragraph_parser_end(s);
    } else if(pch->c == ' ' || pch->c == '\t') {
        if(s->out_of_word == 0) s->out_of_word = 1;
    } else if(pch->c == '\n') {
        if(s->out_of_word < 2) {
            s->out_of_word = 2;
        } else if(s->out_of_word == 2) {
            paragraph_parser_end(s);
        }
    } else {
        if(s->out_of_word == 1 || s->out_of_word == 2) {
            out_stream_write_str(s->out_stream, " ");
        } else if(s->out_of_word == 3) {
            out_stream_write_str(s->out_stream, "        <p>");
        }

        out_stream_write_char(s->out_stream, pch->c);
        s->out_of_word = 0;
    }

    pch->parsed = true;
    pch->move_count = 1;
}

void paragraph_parser_end(struct paragraph_parser *s) {
    if(s->out_of_word < 3) {
        out_stream_write_str(s->out_stream, "</p>\n");
        s->out_of_word = 3;
    }
}
