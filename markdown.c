#include <stdbool.h>

#include "htmd.h"
#include "markdown.h"

void markdown_parser_init(struct markdown_parser *s, char *proc_name) {
	out_stream_init(&s->out_stream, proc_name);
	out_stream_write_str(&s->out_stream, "<!DOCTYPE html>\n<html>\n    <head>\n        <link rel=\"stylesheet\" href=\"style.css\">\n    </head>\n    <body>\n");
}

void markdown_parse(struct markdown_parser *s, struct parser_char *pch) {
	pch->parsed = true;
	pch->move_count = 1;
}

void markdown_parse_force(char c) {

}

void markdown_parser_close(struct markdown_parser *s) {
	out_stream_write_str(&s->out_stream, "    </body>\n</html>\n");
	out_stream_flush(&s->out_stream);
}
