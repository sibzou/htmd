#include <stdbool.h>

#include "misc.h"
#include "htmd.h"
#include "outstream.h"
#include "html.h"

void html_init(struct html *s) {
    out_stream_init(&s->out_stream);
}

void html_write_header(struct html *s) {
    out_stream_write_str(&s->out_stream, "<!DOCTYPE html>\n<html>\n    <head>\n        <link rel=\"stylesheet\" href=\"style.css\">\n    </head>\n    <body>\n");
}

void html_write_footer(struct html *s) {
    out_stream_write_str(&s->out_stream, "    </body>\n</html>\n");
    out_stream_flush(&s->out_stream);
}

void html_handle_markdown_result(struct html *s, struct parser_char *pch) {
    if(pch->res == PCR_SPACE) {
        out_stream_write_char(&s->out_stream, ' ');
    } else if(pch->res == PCR_PARAG_BEGIN) {
        out_stream_write_str(&s->out_stream, "        <p>");
    } else if(pch->res == PCR_PARAG_END) {
        out_stream_write_str(&s->out_stream, "</p>\n");
    } else if(pch->res == PCR_LINK_URL_BEGIN) {
        out_stream_write_str(&s->out_stream, "<a href=\"");
    } else if(pch->res == PCR_SPACE_AND_LINK_URL_BEGIN) {
        out_stream_write_str(&s->out_stream, " <a href=\"");
    } else if(pch->res == PCR_LINK_TEXT_BEGIN) {
        out_stream_write_str(&s->out_stream, "\">");
    } else if(pch->res == PCR_LINK_END) {
        out_stream_write_str(&s->out_stream, "</a>");
    } else if(pch->res == PCR_CHAR) {
        out_stream_write_char(&s->out_stream, pch->c);
    }
}
