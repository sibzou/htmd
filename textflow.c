#include <stdbool.h>

#include "misc.h"
#include "htmd.h"
#include "textflow.h"
#include "outstream.h"

void text_flow_parser_init(struct text_flow_parser *s,
        struct out_stream *out_stream) {

    s->out_stream = out_stream;
    text_flow_parser_reset(s);
}

void text_flow_parser_reset(struct text_flow_parser *s) {
    s->step = TFPS_WAIT;
}

bool text_flow_parse_char(struct text_flow_parser *s, char c) {
    if(c == ' ' || c == '\t' || c == '\n') {
        if(s->step == TFPS_WORD_IN) {
            s->step = TFPS_WORD_OUT;
        }
    } else {
        if(s->step == TFPS_WAIT) {
            s->step = TFPS_WORD_IN;
            return true;
        } else if(s->step == TFPS_WORD_OUT) {
            s->step = TFPS_WORD_IN;
            out_stream_write_char(s->out_stream, ' ');
        }

        out_stream_write_char(s->out_stream, c);
    }

    return false;
}
