struct parser_char {
    char c;
    bool end;

    bool parsed;
    int move_count;
};

struct input_buffer {
    char read_buf[BUFFER_SIZE];
    int read_buf_len;
    int read_buf_unparse_start;

    char backlog[BUFFER_SIZE];
    int backlog_start;
    int backlog_len;

    int user_cursor;
};
