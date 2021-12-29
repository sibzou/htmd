#include <stdbool.h>

#include "misc.h"
#include "htmd.h"
#include "outstream.h"
#include "paragraph.h"

void paragraph_parser_init(struct paragraph_parser *s, struct out_stream *out_stream) {
    s->out_stream = out_stream;
    s->step = PPS_OUT_PARAG;
}

static bool paragraph_parse_char(struct paragraph_parser *s, char c) {
    if(c == ' ' || c == '\t') {
        if(s->step == PPS_IN_WORD) s->step = PPS_OUT_WORD;
    } else if(c == '\n') {
        if(s->step == PPS_IN_WORD || s->step == PPS_OUT_WORD) {
            s->step = PPS_OUT_LINE;
            return true;
        } else if(s->step == PPS_OUT_LINE) {
            paragraph_parser_end(s);
        }
    } else {
        if(s->step == PPS_OUT_WORD || s->step == PPS_OUT_LINE) {
            out_stream_write_str(s->out_stream, " ");
        } else if(s->step == PPS_OUT_PARAG) {
            out_stream_write_str(s->out_stream, "        <p>");
        }

        out_stream_write_char(s->out_stream, c);
        s->step = PPS_IN_WORD;
    }

    return false;
}

bool paragraph_parse(struct paragraph_parser *s, struct parser_char *pch) {
    pch->parsed = true;
    pch->move_count = 1;

    if(pch->end) {
        if(s->step != PPS_OUT_PARAG) {
            paragraph_parser_end(s);
        }
    } else {
        return paragraph_parse_char(s, pch->c);
    }

    return false;
}

void paragraph_parse_force(struct paragraph_parser *s, char c) {
    paragraph_parse_char(s, c);
}

void paragraph_parser_end(struct paragraph_parser *s) {
    out_stream_write_str(s->out_stream, "</p>\n");
    s->step = PPS_OUT_PARAG;
}
