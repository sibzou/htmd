struct text_flow_parser {
    enum {
        TFPS_IN_WORD,
        TFPS_OUT_WORD,
        LKPS_TEXT,
        LKPS_CLOSE_BRACKET,
        LKPS_OPEN_PARENTHESIS
    } step;

    int link_start_distance;
    struct out_stream *out_stream;
};

void text_flow_parser_init(struct text_flow_parser *s,
        struct out_stream *out_stream);

bool text_flow_parse_start(struct text_flow_parser *s,
        struct parser_char *pch);

void text_flow_parse(struct text_flow_parser *s, struct parser_char *pch);
