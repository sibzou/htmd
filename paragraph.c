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
}

bool paragraph_parse_start(struct paragraph_parser *s,
        struct parser_char *pch) {

    bool res = text_flow_parse_start(&s->text_flow_parser, pch);

    if(res) {
        out_stream_write_str(s->out_stream, "        <p>");
    }

    return res;
}

bool paragraph_parse(struct paragraph_parser *s, struct parser_char *pch) {
    pch->parsed = true;
    pch->move_count = 1;

    if(pch->type == PCT_END) {
        paragraph_parser_end(s);
        return false;
    } else {
        int prev_text_flow_parser_step = s->text_flow_parser.step;
        text_flow_parse(&s->text_flow_parser, pch);

        if(s->text_flow_parser.step == TFPS_OUT_WORD) {
            if(prev_text_flow_parser_step != TFPS_OUT_WORD) {
                s->out_line = false;
            }

            if(pch->c == '\n') {
                if(s->out_line) {
                    paragraph_parser_end(s);
                    return false;
                } else {
                    s->out_line = true;
                }
            }
        }
    }

    return true;
}
