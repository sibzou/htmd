struct parser_char {
    char c;
    enum {
        PCT_NORMAL,
        PCT_END,
        PCT_FORCED
    } type;

    int pos;
    bool parsed;

    int move_count; // deprecated
};

struct backlog {
    char buf[BUFFER_SIZE];
    int start;
    int len;
};
