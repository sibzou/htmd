struct link_parser {
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
    } step;

    int url_start;
    int url_end;
    int text_start;
    int text_end;
    int end;
};

struct text_flow_parser {
    enum {
        TFS_FLOW_OUT,
        TFS_WORD_IN,
        TFS_WORD_OUT
    } step;

    struct link_parser link;
};

void text_flow_parser_init(struct text_flow_parser *s);
void text_flow_parse(struct text_flow_parser *s, struct parser_char *pch);
