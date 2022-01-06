struct paragraph_parser {
    struct text_flow_parser text_flow_parser;
    bool out_line;
    struct out_stream *out_stream;
};

void paragraph_parser_init(struct paragraph_parser *s,
        struct out_stream *out_stream);

bool paragraph_parse_start(struct paragraph_parser *s,
        struct parser_char *pch);

bool paragraph_parse(struct paragraph_parser *s, struct parser_char *pch);
