struct markdown_parser {
    struct paragraph_parser paragraph_parser;
};

void markdown_parser_init(struct markdown_parser *s);
void markdown_parse(struct markdown_parser *s, struct parser_char *pch);
