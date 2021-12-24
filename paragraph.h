struct paragraph_parser {
    struct out_stream *out_stream;
    enum {
        PPS_IN_WORD,
        PPS_OUT_WORD,
        PPS_OUT_LINE,
        PPS_OUT_PARAG
    } step;
};

void paragraph_parser_init(struct paragraph_parser *s, struct out_stream *out_stream);
bool paragraph_parse(struct paragraph_parser *s, struct parser_char *pch);
void paragraph_parse_force(struct paragraph_parser *s, char c);
void paragraph_parser_end(struct paragraph_parser *s);
