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
        s->link_step = LKS_TEXT;
        s->in_word = true;

        s->link_start_distance = 0;
        pch->parsed = false;
        pch->move_count = 1;
        return true;
    } else {
        return false;
    }
}

static void link_cancel_parse(struct text_flow_parser *s,
        struct parser_char *pch) {

    s->link_step = LKS_NONE;
    pch->parsed = false;
    pch->move_count = s->link_start_distance;
}

static void prepare_for_link_url_write(struct text_flow_parser *s,
        struct parser_char *pch) {

    out_stream_write_str(s->out_stream, "<a href=\"");
    s->link_step = LKS_WRITE_URL;
    s->link_end = -s->link_start_distance + 1;
    pch->move_count = s->link_start_distance + s->link_url_start;
}

void link_parse(struct text_flow_parser *s, struct parser_char *pch) {
    pch->parsed = false;
    pch->move_count = 1;

    if(pch->type == PCT_END || pch->c == '\n') {
        link_cancel_parse(s, pch);
    } else if(s->link_step == LKS_TEXT) {
        if(pch->c != ' ' && pch->c != '\t') {
            s->link_step = LKS_CLOSE_BRACKET;
            s->link_text_start = -s->link_start_distance;
        }
    } else if(s->link_step == LKS_CLOSE_BRACKET) {
        if(pch->c == ']') {
            s->link_step = LKS_OPEN_PARENTHESIS;
            s->link_text_end = -s->link_start_distance;
        }
    } else if(s->link_step == LKS_OPEN_PARENTHESIS) {
        if(pch->c == '(') {
            s->link_step = LKS_URL;
        } else {
            link_cancel_parse(s, pch);
        }
    } else if(s->link_step == LKS_URL) {
        if(pch->c != ' ' && pch->c != '\t') {
            s->link_step = LKS_URL_END;
            s->link_url_start = -s->link_start_distance;
        }
    } else if(s->link_step == LKS_URL_END) {
        if(pch->c == ')') {
            s->link_url_end = -s->link_start_distance;
            prepare_for_link_url_write(s, pch);
        } else if(pch->c == ' ') {
            s->link_step = LKS_CLOSE_PARENTHESIS;
            s->link_url_end = -s->link_start_distance;
        }
    } else if(s->link_step == LKS_CLOSE_PARENTHESIS) {
        if(pch->c == ')') {
            prepare_for_link_url_write(s, pch);
        } else if(pch->c != ' ' && pch->c != '\t') {
            link_cancel_parse(s, pch);
        }
    } else if(s->link_step == LKS_WRITE_URL) {
        if(s->link_start_distance == -s->link_url_end) {
            out_stream_write_str(s->out_stream, "\">");
            s->link_step = LKS_WRITE_TEXT;
            pch->move_count = s->link_text_start - s->link_url_end;
        } else {
            out_stream_write_char(s->out_stream, pch->c);
        }
    } else if(s->link_step == LKS_WRITE_TEXT) {
        if(s->link_start_distance == -s->link_text_end) {
            out_stream_write_str(s->out_stream, "</a>");
            s->link_step = LKS_NONE;
            pch->parsed = true;
            pch->move_count = s->link_end - s->link_text_end;
        }
    }
}

bool text_flow_parse_start(struct text_flow_parser *s,
        struct parser_char *pch) {

    bool res = link_parse_start(s, pch);

    if(!res) {
        res = is_a_word_char(pch->c);

        if(res) {
            s->in_word = true;
            pch->parsed = false;
            pch->move_count = 0;
        }
    }

    s->link_start_distance -= pch->move_count;
    return res;
}

void text_flow_parse(struct text_flow_parser *s, struct parser_char *pch) {
    pch->parsed = true;
    pch->move_count = 1;

    int unch_link_step = s->link_step; // unchanged link step

    if(unch_link_step != LKS_NONE) {
        link_parse(s, pch);
    }

    if(unch_link_step == LKS_NONE || (unch_link_step == LKS_WRITE_TEXT
            && s->link_step != LKS_NONE)) {
        if(s->in_word) {
            if(is_a_word_char(pch->c)) {
                out_stream_write_char(s->out_stream, pch->c);
            } else {
                s->in_word = false;
            }
        } else {
            if(is_a_word_char(pch->c)) {
                out_stream_write_char(s->out_stream, ' ');

                if(unch_link_step == LKS_WRITE_TEXT
                        || !link_parse_start(s, pch)) {

                    s->in_word = true;
                    pch->parsed = false;
                    pch->move_count = 0;
                }
            }
        }
    }

    s->link_start_distance -= pch->move_count;
}

void text_flow_parser_prepare_for_forced_chars(struct text_flow_parser *s) {
    s->link_step = LKS_NONE;
}
