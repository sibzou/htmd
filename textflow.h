struct text_flow_parser {
    enum {
        TFPS_WAIT,
        TFPS_WORD_IN,
        TFPS_WORD_OUT
    } step;
    struct out_stream *out_stream;
};

void text_flow_parser_init(struct text_flow_parser *s,
    struct out_stream *out_stream);

void text_flow_parser_reset(struct text_flow_parser *s);
bool text_flow_parse_char(struct text_flow_parser *s, char c);
