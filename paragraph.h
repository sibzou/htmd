struct paragraph_parser {
    struct text_flow_parser text_flow_parser;
    bool out_line;
};

void paragraph_parser_init(struct paragraph_parser *s);
void paragraph_parse(struct paragraph_parser *s, struct parser_char *pch);
