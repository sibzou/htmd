struct in_stream {
    char buf[BUFFER_SIZE];
    int buf_len;
    int read_head;
};

void in_stream_init(struct in_stream *s);
bool in_stream_read(struct in_stream *s, char *c);
