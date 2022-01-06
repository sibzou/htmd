#include <stdbool.h>

#include "misc.h"
#include "htmd.h"
#include "textflow.h"
#include "outstream.h"

void text_flow_parser_init(struct text_flow_parser *s,
        struct out_stream *out_stream) {

    s->out_stream = out_stream;
}

static bool is_a_word_char(char c) {
    return c != ' ' && c != '\t' && c != '\n';
}

static bool link_parse_start(struct text_flow_parser *s,
        struct parser_char *pch) {

    if(pch->type != PCT_FORCED && pch->c == '[') {
        s->step = LKPS_TEXT;
        s->link_start_distance = -1;
        pch->parsed = false;
        pch->move_count = 1;
        return true;
    } else {
        return false;
    }
}

static void link_cancel_parse(struct text_flow_parser *s,
        struct parser_char *pch) {

    s->step = TFPS_IN_WORD;
    pch->parsed = false;
    pch->move_count = s->link_start_distance;
}

void link_parse(struct text_flow_parser *s, struct parser_char *pch) {
    pch->parsed = false;
    pch->move_count = 1;

    if(pch->type == PCT_FORCED || pch->type == PCT_END
            || pch->c == '\n') {

        link_cancel_parse(s, pch);
        pch->move_count = 0;
        return;
    } else if(s->step == LKPS_TEXT) {
        if(pch->c != ' ' && pch->c != '\t') {
            s->step = LKPS_CLOSE_BRACKET;
        }
    } else if(s->step == LKPS_CLOSE_BRACKET) {
        if(pch->c == ']') {
            s->step = LKPS_OPEN_PARENTHESIS;
        }
    } else if(s->step == LKPS_OPEN_PARENTHESIS) {
        if(pch->c == '(') {
            out_stream_write_str(s->out_stream, "<a></a>");
            s->step = TFPS_OUT_WORD;
            pch->parsed = true;
        } else {
            link_cancel_parse(s, pch);
        }
    }

    s->link_start_distance--;
}

bool text_flow_parse_start(struct text_flow_parser *s,
        struct parser_char *pch) {

    bool res = link_parse_start(s, pch);

    if(!res) {
        res = is_a_word_char(pch->c);

        if(res) {
            s->step = TFPS_IN_WORD;
            pch->parsed = false;
            pch->move_count = 0;
        }
    }

    return res;
}

void text_flow_parse(struct text_flow_parser *s, struct parser_char *pch) {
    pch->parsed = true;
    pch->move_count = 1;

    if(s->step == TFPS_OUT_WORD) {
        if(is_a_word_char(pch->c)) {
            out_stream_write_char(s->out_stream, ' ');
        }

        if(!link_parse_start(s, pch) && is_a_word_char(pch->c)) {
            s->step = TFPS_IN_WORD;
            pch->parsed = false;
            pch->move_count = 0;
        }
    } else if(s->step == TFPS_IN_WORD) {
        if(is_a_word_char(pch->c)) {
            out_stream_write_char(s->out_stream, pch->c);
        } else {
            s->step = TFPS_OUT_WORD;
        }
    } else {
        link_parse(s, pch);
    }
}
