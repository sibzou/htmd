#include <stdbool.h>

#include "misc.h"
#include "htmd.h"
#include "outstream.h"
#include "textflow.h"
#include "paragraph.h"
#include "markdown.h"

void markdown_parser_init(struct markdown_parser *s) {
    paragraph_parser_init(&s->paragraph_parser);
}

void markdown_parse(struct markdown_parser *s, struct parser_char *pch) {
    paragraph_parse(&s->paragraph_parser, pch);
}
