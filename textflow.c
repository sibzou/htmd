#include <stdbool.h>

#include "misc.h"
#include "htmd.h"
#include "textflow.h"
#include "outstream.h"

static void link_parser_reset(struct text_flow_parser *s) {
    s->link_step = LKS_OPEN_BRACKET;
}

void text_flow_parser_init(struct text_flow_parser *s) {
    s->step = TFS_FLOW_OUT;
    link_parser_reset(s);
}

static void link_cancel_parse(struct text_flow_parser *s,
        struct parser_char *pch) {

    s->link_step = LKS_NONE;
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

        s->link_step = LKS_WRITE_URL;
        s->link_end = pch->pos + 1;

        pch->next_pos = s->link_url_start;
    }
}

static bool link_parse_text(struct text_flow_parser *s,
        struct parser_char *pch) {

    if(s->link_step == LKS_TEXT) {
        if(pch->c != ' ' && pch->c != '\t') {
            s->link_step = LKS_CLOSE_BRACKET;
            s->link_text_start = pch->pos;
        }
    } else if(s->link_step == LKS_CLOSE_BRACKET) {
        if(pch->c == ']') {
            s->link_step = LKS_OPEN_PARENTHESIS;
            s->link_text_end = pch->pos;
        }
    } else {
        return false;
    }

    return true;
}

static bool link_parse_url_begin(struct text_flow_parser *s,
        struct parser_char *pch) {

    if(s->link_step == LKS_OPEN_PARENTHESIS) {
        if(pch->c == '(') {
            s->link_step = LKS_URL;
        } else {
            link_cancel_parse(s, pch);
        }
    } else if(s->link_step == LKS_URL) {
        if(pch->c != ' ' && pch->c != '\t') {
            s->link_step = LKS_URL_END;
            s->link_url_start = pch->pos;
        }
    } else {
        return false;
    }

    return true;
}

static bool link_parse_url_end(struct text_flow_parser *s,
        struct parser_char *pch) {

    if(s->link_step == LKS_URL_END) {
        if(pch->c == ')') {
            s->link_url_end = pch->pos;
            prepare_for_link_url_write(s, pch);
        } else if(pch->c == ' ') {
            s->link_step = LKS_CLOSE_PARENTHESIS;
            s->link_url_end = pch->pos;
        }
    } else if(s->link_step == LKS_CLOSE_PARENTHESIS) {
        if(pch->c == ')') {
            prepare_for_link_url_write(s, pch);
        } else if(pch->c != ' ' && pch->c != '\t') {
            link_cancel_parse(s, pch);
        }
    } else {
        return false;
    }

    return true;
}

static void link_control_write(struct text_flow_parser *s,
        struct parser_char *pch) {

    if(s->link_step == LKS_WRITE_URL) {
        if(pch->pos == s->link_url_end) {
            pch->res = PCR_LINK_TEXT_BEGIN;
            s->link_step = LKS_CONTROL_TEXT;
            pch->next_pos = s->link_text_start;
        } else {
            pch->res = PCR_CHAR;
        }
    } else if(s->link_step == LKS_CONTROL_TEXT) {
        if(pch->pos == s->link_text_end) {
            pch->res = PCR_LINK_END;
            link_parser_reset(s);

            pch->next_pos = s->link_end;
            pch->parsed = true;
        } else {
            s->link_step = LKS_WRITE_TEXT;
            pch->next_pos = pch->pos;
        }
    }
}

static void link_parse(struct text_flow_parser *s, struct parser_char *pch) {
    pch->parsed = false;
    pch->next_pos = pch->pos + 1;
    pch->res = PCR_NONE;

    if(pch->type == PCT_FORCED) {
        s->link_step = LKS_NONE;
        pch->next_pos = pch->pos;
    } else if(pch->type == PCT_END || pch->c == '\n') {
        link_cancel_parse(s, pch);
    } else if(s->link_step == LKS_OPEN_BRACKET) {
        if(pch->type != PCT_FORCED && pch->c == '[') {
            s->link_step = LKS_TEXT;
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
        pch->parsed = s->link_step != LKS_WRITE_TEXT;
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
                    pch->res = PCR_SPACE;
                }

                s->step = TFS_WORD_IN;
                pch->parsed = false;
                pch->next_pos = pch->pos;
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
