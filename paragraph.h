struct paragraph_parser {
    struct text_flow_parser text_flow_parser;
    bool out_line;
    struct out_stream *out_stream;
};

void paragraph_parser_init(struct paragraph_parser *s, struct out_stream *out_stream);
void paragraph_parse(struct paragraph_parser *s, struct parser_char *pch);
void paragraph_parse_force(struct paragraph_parser *s, char c);
