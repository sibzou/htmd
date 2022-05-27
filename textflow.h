struct text_flow_parser {
    enum {
        TFS_FLOW_OUT,
        TFS_WORD_IN,
        TFS_WORD_OUT
    } step;

    enum {
        LKS_NONE,
        LKS_OPEN_BRACKET,
        LKS_TEXT,
        LKS_CLOSE_BRACKET,
        LKS_OPEN_PARENTHESIS,
        LKS_URL,
        LKS_URL_END,
        LKS_CLOSE_PARENTHESIS,
        LKS_WRITE_URL,
        LKS_CONTROL_TEXT,
        LKS_WRITE_TEXT
    } link_step;

    int link_url_start;
    int link_url_end;
    int link_text_start;
    int link_text_end;
    int link_end;
};

void text_flow_parser_init(struct text_flow_parser *s);
void text_flow_parse(struct text_flow_parser *s, struct parser_char *pch);
