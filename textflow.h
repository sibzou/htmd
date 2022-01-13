struct text_flow_parser {
    bool in_word;

    enum {
        LKS_NONE,
        LKS_TEXT,
        LKS_CLOSE_BRACKET,
        LKS_OPEN_PARENTHESIS,
        LKS_URL,
        LKS_URL_END,
        LKS_CLOSE_PARENTHESIS,
        LKS_WRITE_URL,
        LKS_WRITE_TEXT
    } link_step;

    int link_start_distance;
    int link_url_start;
    int link_url_end;
    int link_text_start;
    int link_text_end;
    int link_end;
    struct out_stream *out_stream;
};

void text_flow_parser_init(struct text_flow_parser *s,
        struct out_stream *out_stream);

bool text_flow_parse_start(struct text_flow_parser *s,
        struct parser_char *pch);

void text_flow_parse(struct text_flow_parser *s, struct parser_char *pch);

void text_flow_parser_prepare_for_forced_chars(struct text_flow_parser *s);
