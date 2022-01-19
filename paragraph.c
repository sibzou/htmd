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

void paragraph_parse(struct paragraph_parser *s, struct parser_char *pch) {
    int prev_step = s->text_flow_parser.step;
    text_flow_parse(&s->text_flow_parser, pch);

    if(prev_step == TFS_FLOW_OUT && s->text_flow_parser.step == TFS_WORD_IN) {
        out_stream_write_str(s->out_stream, "        <p>");
    } else if(s->text_flow_parser.step != TFS_FLOW_OUT) {
        if(s->text_flow_parser.step == TFS_WORD_OUT) {
            if(prev_step == TFS_WORD_IN) {
                s->out_line = false;
            }

            if(pch->type != PCT_END && pch->c == '\n' && pch->parsed) {
                if(s->out_line) {
                    paragraph_parser_end(s);
                    text_flow_parser_reset(&s->text_flow_parser);
                } else {
                    s->out_line = true;
                }
            }
        }

        if(pch->type == PCT_END && pch->parsed) {
            paragraph_parser_end(s);
        }
    }
}

void paragraph_parser_prepare_for_forced_chars(struct paragraph_parser *s) {
    text_flow_parser_prepare_for_forced_chars(&s->text_flow_parser);
}
