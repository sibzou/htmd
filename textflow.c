#include <stdbool.h>

#include "misc.h"
#include "htmd.h"
#include "textflow.h"
#include "outstream.h"

static void link_parser_reset(struct link_parser *s) {
    s->step = LKS_OPEN_BRACKET;
}

void text_flow_parser_init(struct text_flow_parser *s) {
    s->step = TFS_FLOW_OUT;
    link_parser_reset(&s->link);
}

static void link_cancel_parse(struct link_parser *s, struct parser_char *pch) {
    s->step = LKS_NONE;
    pch->next_pos = 0;
}

static void prepare_for_link_url_write(struct text_flow_parser *s,
        struct parser_char *pch) {

    if(s->step == TFS_FLOW_OUT) {
        s->step = TFS_WORD_IN;
        pch->next_pos = pch->pos;
    } else {
        if(s->step == TFS_WORD_OUT) {
            pch->res = PCR_SPACE_AND_LINK_URL_BEGIN;
            s->step = TFS_WORD_IN;
        } else {
            pch->res = PCR_LINK_URL_BEGIN;
        }

        s->link.step = LKS_WRITE_URL;
        s->link.end = pch->pos + 1;

        pch->next_pos = s->link.url_start;
    }
}

static bool link_parse_text(struct link_parser *s, struct parser_char *pch) {
    if(s->step == LKS_TEXT) {
        if(pch->c != ' ' && pch->c != '\t') {
            s->step = LKS_CLOSE_BRACKET;
            s->text_start = pch->pos;
        }
    } else if(s->step == LKS_CLOSE_BRACKET) {
        if(pch->c == ']') {
            s->step = LKS_OPEN_PARENTHESIS;
            s->text_end = pch->pos;
        }
    } else {
        return false;
    }

    return true;
}

static bool link_parse_url_begin(struct link_parser *s,
        struct parser_char *pch) {

    if(s->step == LKS_OPEN_PARENTHESIS) {
        if(pch->c == '(') {
            s->step = LKS_URL;
        } else {
            link_cancel_parse(s, pch);
        }
    } else if(s->step == LKS_URL) {
        if(pch->c != ' ' && pch->c != '\t') {
            s->step = LKS_URL_END;
            s->url_start = pch->pos;
        }
    } else {
        return false;
    }

    return true;
}

static bool link_parse_url_end(struct text_flow_parser *s,
        struct parser_char *pch) {

    if(s->link.step == LKS_URL_END) {
        if(pch->c == ')') {
            s->link.url_end = pch->pos;
            prepare_for_link_url_write(s, pch);
        } else if(pch->c == ' ') {
            s->link.step = LKS_CLOSE_PARENTHESIS;
            s->link.url_end = pch->pos;
        }
    } else if(s->link.step == LKS_CLOSE_PARENTHESIS) {
        if(pch->c == ')') {
            prepare_for_link_url_write(s, pch);
        } else if(pch->c != ' ' && pch->c != '\t') {
            link_cancel_parse(&s->link, pch);
        }
    } else {
        return false;
    }

    return true;
}

static void link_control_write(struct link_parser *s, struct parser_char *pch) {
    if(s->step == LKS_WRITE_URL) {
        if(pch->pos == s->url_end) {
            pch->res = PCR_LINK_TEXT_BEGIN;
            s->step = LKS_WRITE_TEXT;
            pch->next_pos = s->text_start;
        } else {
            pch->res = PCR_CHAR;
        }
    } else if(s->step == LKS_WRITE_TEXT) {
        if(pch->pos == s->text_end) {
            pch->res = PCR_LINK_END;
            link_parser_reset(s);

            pch->next_pos = s->end;
            pch->parsed = true;
        } else {
            pch->res = PCR_CHAR;
        }
    }
}

static void link_parse(struct text_flow_parser *s, struct parser_char *pch) {
    pch->parsed = false;
    pch->next_pos = pch->pos + 1;
    pch->res = PCR_NONE;

    if(pch->type == PCT_FORCED) {
        s->link.step = LKS_NONE;
        pch->next_pos = pch->pos;
    } else if(pch->type == PCT_END || pch->c == '\n') {
        link_cancel_parse(&s->link, pch);
    } else if(s->link.step == LKS_OPEN_BRACKET) {
        if(pch->type != PCT_FORCED && pch->c == '[') {
            s->link.step = LKS_TEXT;
        } else {
            link_cancel_parse(&s->link, pch);
        }
    } else if(link_parse_text(&s->link, pch)) {}
    else if(link_parse_url_begin(&s->link, pch)) {}
    else if(link_parse_url_end(s, pch)) {}
    else {
        link_control_write(&s->link, pch);
    }
}

static bool is_a_word_char(char c) {
    return c != ' ' && c != '\t' && c != '\n';
}

void text_flow_parse(struct text_flow_parser *s, struct parser_char *pch) {
    if(s->link.step != LKS_NONE) {
        link_parse(s, pch);
    } else {
        pch->parsed = true;
        pch->next_pos = pch->pos + 1;
        pch->res = PCR_NONE;

        if(pch->type == PCT_END) {
            return;
        }

        if(s->step == TFS_WORD_IN) {
            if(is_a_word_char(pch->c)) {
                pch->res = PCR_CHAR;
            } else {
                s->step = TFS_WORD_OUT;
                link_parser_reset(&s->link);
            }
        } else {
            if(is_a_word_char(pch->c)) {
                if(s->step == TFS_WORD_OUT) {
                    pch->res = PCR_SPACE;
                }

                s->step = TFS_WORD_IN;
                pch->parsed = false;
                pch->next_pos = pch->pos;
            }
        }
    }
}
