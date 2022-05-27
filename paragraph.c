#include <stdbool.h>

#include "misc.h"
#include "htmd.h"
#include "outstream.h"
#include "textflow.h"
#include "paragraph.h"

void paragraph_parser_init(struct paragraph_parser *s) {
    text_flow_parser_init(&s->text_flow_parser);
}

void paragraph_parse(struct paragraph_parser *s, struct parser_char *pch) {
    int prev_step = s->text_flow_parser.step;
    text_flow_parse(&s->text_flow_parser, pch);

    if(prev_step == TFS_FLOW_OUT && s->text_flow_parser.step == TFS_WORD_IN) {
        pch->res = PCR_PARAG_BEGIN;
    } else if(s->text_flow_parser.step != TFS_FLOW_OUT) {
        if(s->text_flow_parser.step == TFS_WORD_OUT) {
            if(prev_step == TFS_WORD_IN) {
                s->out_line = false;
            }

            if(pch->type != PCT_END && pch->c == '\n' && pch->parsed) {
                if(s->out_line) {
                    pch->res = PCR_PARAG_END;
                    text_flow_parser_init(&s->text_flow_parser);
                } else {
                    s->out_line = true;
                }
            }
        }

        if(pch->type == PCT_END && pch->parsed) {
            pch->res = PCR_PARAG_END;
        }
    }
}
