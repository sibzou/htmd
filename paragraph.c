#include <stdbool.h>

#include "misc.h"
#include "htmd.h"
#include "outstream.h"
#include "textflow.h"
#include "paragraph.h"

void paragraph_parser_init(struct paragraph_parser *s,
        struct out_stream *out_stream) {

    text_flow_parser_init(&s->text_flow_parser, out_stream);
    s->out_stream = out_stream;
}

static void paragraph_parser_end(struct paragraph_parser *s) {
    out_stream_write_str(s->out_stream, "</p>\n");
    text_flow_parser_reset(&s->text_flow_parser);
}

void paragraph_parse(struct paragraph_parser *s, struct parser_char *pch) {
    int prev_text_flow_parser_step = s->text_flow_parser.step;
    text_flow_parse(&s->text_flow_parser, pch);

    if(pch->type == PCT_END) {
        if(s->text_flow_parser.step != TFPS_WAIT && pch->move_count > 0) {
            paragraph_parser_end(s);
        }

        return;
    }

    if(prev_text_flow_parser_step == TFPS_WAIT
            && s->text_flow_parser.step != TFPS_WAIT) {

        out_stream_write_str(s->out_stream, "        <p>");
    } else if(prev_text_flow_parser_step != TFPS_WORD_OUT
            && s->text_flow_parser.step == TFPS_WORD_OUT) {

        s->out_line = false;
    }

    if(pch->c == '\n' && s->text_flow_parser.step == TFPS_WORD_OUT) {
        if(s->out_line) {
            paragraph_parser_end(s);
        } else {
            s->out_line = true;
        }
    }
}
