#include <stdbool.h>

#include "misc.h"
#include "htmd.h"
#include "textflow.h"
#include "outstream.h"

void text_flow_parser_init(struct text_flow_parser *s,
        struct out_stream *out_stream) {

    s->out_stream = out_stream;
}

static bool is_not_a_word_char(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

bool text_flow_parse_start(struct text_flow_parser *s,
        struct parser_char *pch) {

    s->in_word = !is_not_a_word_char(pch->c);

    if(s->in_word) {
        pch->parsed = false;
        pch->move_count = 0;
    }

    return s->in_word;
}

void text_flow_parse(struct text_flow_parser *s, struct parser_char *pch) {
    pch->parsed = true;
    pch->move_count = 1;

    if(is_not_a_word_char(pch->c)) {
        if(s->in_word) {
            s->in_word = false;
        }
    } else {
        if(!s->in_word) {
            s->in_word = true;
            out_stream_write_char(s->out_stream, ' ');
        }

        out_stream_write_char(s->out_stream, pch->c);
    }
}
