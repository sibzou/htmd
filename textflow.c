#include <stdbool.h>

#include "misc.h"
#include "htmd.h"
#include "textflow.h"
#include "outstream.h"

void text_flow_parser_init(struct text_flow_parser *s,
        struct out_stream *out_stream) {

    text_flow_parser_reset(s);
    s->out_stream = out_stream;
}

static void link_parser_reset(struct text_flow_parser *s) {
    s->link_step = LKS_OPEN_BRACKET;
    s->link_start_distance = 0;
}

void text_flow_parser_reset(struct text_flow_parser *s) {
    s->step = TFS_FLOW_OUT;
    link_parser_reset(s);
}

static void link_cancel_parse(struct text_flow_parser *s,
        struct parser_char *pch) {

    s->link_step = LKS_NONE;
    pch->move_count = s->link_start_distance;
}

static void prepare_for_link_url_write(struct text_flow_parser *s,
        struct parser_char *pch) {

    if(s->step == TFS_FLOW_OUT) {
        s->step = TFS_WORD_IN;
        pch->move_count = 0;
    } else {
        if(s->step == TFS_WORD_OUT) {
            out_stream_write_char(s->out_stream, ' ');
            s->step = TFS_WORD_IN;
        }

        out_stream_write_str(s->out_stream, "<a href=\"");
        s->link_step = LKS_WRITE_URL;
        s->link_end = -s->link_start_distance + 1;

        pch->move_count = s->link_start_distance + s->link_url_start;
        s->link_start_distance -= pch->move_count;
    }
}

static bool link_parse_text(struct text_flow_parser *s,
        struct parser_char *pch) {

    if(s->link_step == LKS_TEXT) {
        if(pch->c != ' ' && pch->c != '\t') {
            s->link_step = LKS_CLOSE_BRACKET;
            s->link_text_start = -s->link_start_distance;
        }
    } else if(s->link_step == LKS_CLOSE_BRACKET) {
        if(pch->c == ']') {
            s->link_step = LKS_OPEN_PARENTHESIS;
            s->link_text_end = -s->link_start_distance;
        }
    } else {
        return false;
    }

    s->link_start_distance--;
    return true;
}

static bool link_parse_url_begin(struct text_flow_parser *s,
        struct parser_char *pch) {

    if(s->link_step == LKS_OPEN_PARENTHESIS) {
        if(pch->c == '(') {
            s->link_step = LKS_URL;
            s->link_start_distance--;
        } else {
            link_cancel_parse(s, pch);
        }
    } else if(s->link_step == LKS_URL) {
        if(pch->c != ' ' && pch->c != '\t') {
            s->link_step = LKS_URL_END;
            s->link_url_start = -s->link_start_distance;
        }

        s->link_start_distance--;
    } else {
        return false;
    }

    return true;
}

static bool link_parse_url_end(struct text_flow_parser *s,
        struct parser_char *pch) {

    if(s->link_step == LKS_URL_END) {
        if(pch->c == ')') {
            s->link_url_end = -s->link_start_distance;
            prepare_for_link_url_write(s, pch);
        } else {
            if(pch->c == ' ') {
                s->link_step = LKS_CLOSE_PARENTHESIS;
                s->link_url_end = -s->link_start_distance;
            }

            s->link_start_distance--;
        }
    } else if(s->link_step == LKS_CLOSE_PARENTHESIS) {
        if(pch->c == ')') {
            prepare_for_link_url_write(s, pch);
        } else if(pch->c != ' ' && pch->c != '\t') {
            link_cancel_parse(s, pch);
        } else {
            s->link_start_distance--;
        }
    } else {
        return false;
    }

    return true;
}

static void link_control_write(struct text_flow_parser *s,
        struct parser_char *pch) {

    if(s->link_step == LKS_WRITE_URL) {
        if(s->link_start_distance == -s->link_url_end) {
            out_stream_write_str(s->out_stream, "\">");
            s->link_step = LKS_CONTROL_TEXT;

            pch->move_count = s->link_text_start - s->link_url_end;
            s->link_start_distance -= pch->move_count;
        } else {
            out_stream_write_char(s->out_stream, pch->c);
            s->link_start_distance--;
        }
    } else if(s->link_step == LKS_CONTROL_TEXT) {
        if(s->link_start_distance == -s->link_text_end) {
            out_stream_write_str(s->out_stream, "</a>");
            link_parser_reset(s);
            pch->move_count = s->link_end - s->link_text_end;
        } else {
            s->link_step = LKS_WRITE_TEXT;
            pch->move_count = 0;
            s->link_start_distance--;
        }
    }
}

static void link_parse(struct text_flow_parser *s, struct parser_char *pch) {
    pch->parsed = false;
    pch->move_count = 1;

    if(pch->type == PCT_END || pch->c == '\n') {
        link_cancel_parse(s, pch);
    } else if(s->link_step == LKS_OPEN_BRACKET) {
        if(pch->type != PCT_FORCED && pch->c == '[') {
            s->link_step = LKS_TEXT;
            s->link_start_distance--;
        } else {
            link_cancel_parse(s, pch);
        }
    } else if(link_parse_text(s, pch)) {}
    else if(link_parse_url_begin(s, pch)) {}
    else if(link_parse_url_end(s, pch)) {}
    else {
        link_control_write(s, pch);
    }
}

static bool is_a_word_char(char c) {
    return c != ' ' && c != '\t' && c != '\n';
}

void text_flow_parse(struct text_flow_parser *s, struct parser_char *pch) {
    if(s->link_step != LKS_NONE && s->link_step != LKS_WRITE_TEXT) {
        link_parse(s, pch);
    } else {
        pch->parsed = true;
        pch->move_count = 1;

        if(pch->type == PCT_END) {
            return;
        }

        if(s->step == TFS_WORD_IN) {
            if(is_a_word_char(pch->c)) {
                out_stream_write_char(s->out_stream, pch->c);
            } else {
                s->step = TFS_WORD_OUT;

                if(s->link_step != LKS_WRITE_TEXT) {
                    link_parser_reset(s);
                }
            }

            if(s->link_step == LKS_WRITE_TEXT) {
                s->link_step = LKS_CONTROL_TEXT;
            }
        } else {
            if(is_a_word_char(pch->c)) {
                if(s->step == TFS_WORD_OUT) {
                    out_stream_write_char(s->out_stream, ' ');
                }

                s->step = TFS_WORD_IN;
                pch->parsed = false;
                pch->move_count = 0;
            } else {
                if(s->link_step == LKS_WRITE_TEXT) {
                    s->link_step = LKS_CONTROL_TEXT;
                } else {
                    link_parser_reset(s);
                }
            }
        }
    }
}

void text_flow_parser_prepare_for_forced_chars(struct text_flow_parser *s) {
    link_parser_reset(s);
}
