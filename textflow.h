struct text_flow_parser {
    enum {
        TFPS_IN_WORD,
        TFPS_OUT_WORD,
        LKPS_TEXT,
        LKPS_CLOSE_BRACKET,
        LKPS_OPEN_PARENTHESIS,
        LKPS_URL,
        LKPS_URL_END,
        LKPS_CLOSE_PARENTHESIS,
        LKPS_WRITE_URL
    } step;

    int link_start_distance;
    int link_url_start;
    int link_url_end;
    int link_end;
    struct out_stream *out_stream;
};

void text_flow_parser_init(struct text_flow_parser *s,
        struct out_stream *out_stream);

bool text_flow_parse_start(struct text_flow_parser *s,
        struct parser_char *pch);

void text_flow_parse(struct text_flow_parser *s, struct parser_char *pch);

void text_flow_parser_prepare_for_forced_chars(struct text_flow_parser *s);
