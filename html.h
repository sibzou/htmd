struct html {
    struct out_stream out_stream;
};

void html_init(struct html *s);
void html_write_header(struct html *s);
void html_write_footer(struct html *s);
void html_handle_markdown_result(struct html *s, struct parser_char *pch);
