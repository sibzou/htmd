#include <stdbool.h>

#include "htmd.h"
#include "misc.h"
#include "outstream.h"
#include "paragraph.h"
#include "markdown.h"

void markdown_parser_init(struct markdown_parser *s) {
    paragraph_parser_init(&s->paragraph_parser, &s->out_stream);

    out_stream_init(&s->out_stream);
    out_stream_write_str(&s->out_stream, "<!DOCTYPE html>\n<html>\n    <head>\n        <link rel=\"stylesheet\" href=\"style.css\">\n    </head>\n    <body>\n");
}

void markdown_parse(struct markdown_parser *s, struct parser_char *pch) {
    paragraph_parse(&s->paragraph_parser, pch);
}

void markdown_parse_force(char c) {

}

void markdown_parser_close(struct markdown_parser *s) {
    out_stream_write_str(&s->out_stream, "    </body>\n</html>\n");
    out_stream_flush(&s->out_stream);
}
