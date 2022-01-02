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

static void paragraph_parse_char(struct paragraph_parser *s, char c) {
    int prev_text_flow_parser_step = s->text_flow_parser.step;

    if(text_flow_parse_char(&s->text_flow_parser, c)) {
        out_stream_write_str(s->out_stream, "        <p>");
        s->out_line = false;
        text_flow_parse_char(&s->text_flow_parser, c);
    }

    if(s->text_flow_parser.step == TFPS_WORD_OUT
            && prev_text_flow_parser_step != TFPS_WORD_OUT) {

        s->out_line = false;
    }

    if(c == '\n' && s->text_flow_parser.step == TFPS_WORD_OUT) {
        if(s->out_line) {
            paragraph_parser_end(s);
        } else {
            s->out_line = true;
        }
    }
}

void paragraph_parse(struct paragraph_parser *s, struct parser_char *pch) {
    pch->parsed = true;
    pch->move_count = 1;

    if(pch->end) {
        if(s->text_flow_parser.step != TFPS_WAIT) {
            paragraph_parser_end(s);
        }
    } else {
        paragraph_parse_char(s, pch->c);
    }
}

void paragraph_parse_force(struct paragraph_parser *s, char c) {
    paragraph_parse_char(s, c);
}
