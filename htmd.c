#include <stdbool.h>

#include "misc.h"
#include "htmd.h"
#include "textflow.h"
#include "paragraph.h"
#include "instream.h"
#include "outstream.h"
#include "markdown.h"

static void backlog_init(struct backlog *s) {
    s->start = 0;
    s->len = 0;
}

static void force_parse(struct parser_char *pch, struct backlog *backlog,
        struct markdown_parser *mdp) {

    pch->type = PCT_FORCED;
    pch->c = backlog->buf[backlog->start];
    pch->pos = 0;

    do {
        markdown_parse(mdp, pch);
    } while(pch->next_pos == 0);

    backlog->start = (backlog->start + 1) % BUFFER_SIZE;
    backlog->len--;
    pch->pos = pch->next_pos - 1;
}

static void read_in_stream_if_needed(struct parser_char *pch,
        struct backlog *backlog, struct in_stream *in_stream,
        struct markdown_parser *mdp) {

    if(pch->pos < backlog->len) {
        return;
    }

    char c;
    bool is_end = in_stream_read(in_stream, &c);

    if(is_end) {
        return;
    } else if(backlog->len >= BUFFER_SIZE) {
        force_parse(pch, backlog, mdp);
    }

    int offset = (backlog->start + backlog->len) % BUFFER_SIZE;
    backlog->buf[offset] = c;
    backlog->len++;
}

static void get_parser_char(struct parser_char *pch, struct backlog *backlog) {
    if(pch->pos >= backlog->len) {
        pch->type = PCT_END;
    } else {
        pch->type = PCT_NORMAL;
        int offset = (backlog->start + pch->pos) % BUFFER_SIZE;
        pch->c = backlog->buf[offset];
    }
}

static bool handle_parse_result(struct parser_char *pch,
        struct backlog *backlog) {

    pch->pos = pch->next_pos;

    if(pch->type == PCT_END && pch->parsed) {
        return true;
    } else if(pch->parsed) {
        backlog->start = (backlog->start + pch->pos) % BUFFER_SIZE;
        backlog->len -= pch->pos;
        pch->pos = 0;
    }

    return false;
}

int main(int argc, char *argv[]) {
    struct in_stream in_stream;
    struct backlog backlog;
    struct parser_char pch;
    struct markdown_parser mdp;

    in_stream_init(&in_stream);
    backlog_init(&backlog);
    markdown_parser_init(&mdp);

    pch.pos = 0;

    while(true) {
        read_in_stream_if_needed(&pch, &backlog, &in_stream, &mdp);
        get_parser_char(&pch, &backlog);

        markdown_parse(&mdp, &pch);

        if(handle_parse_result(&pch, &backlog)) {
            break;
        }
    }
}
