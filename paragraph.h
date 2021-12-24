struct paragraph_parser {
    struct out_stream *out_stream;
    int out_of_word;
};

void paragraph_parser_init(struct paragraph_parser *s, struct out_stream *out_stream);
void paragraph_parse(struct paragraph_parser *s, struct parser_char *pch);
void paragraph_parse_force(struct paragraph_parser *s, char c);
void paragraph_parser_end(struct paragraph_parser *s);
