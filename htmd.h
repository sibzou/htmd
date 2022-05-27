struct parser_char {
    char c;
    enum {
        PCT_NORMAL,
        PCT_END,
        PCT_FORCED
    } type;
    int pos;

    bool parsed;
    int next_pos;

    enum {
        PCR_NONE,
        PCR_CHAR,
        PCR_SPACE,
        PCR_PARAG_BEGIN,
        PCR_PARAG_END,
        PCR_LINK_URL_BEGIN,
        PCR_SPACE_AND_LINK_URL_BEGIN,
        PCR_LINK_TEXT_BEGIN,
        PCR_LINK_END
    } res;
};

struct backlog {
    char buf[BUFFER_SIZE];
    int start;
    int len;
};
