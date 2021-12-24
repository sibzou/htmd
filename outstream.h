struct out_stream {
    char buf[BUFFER_SIZE];
    int buf_len;
};

void out_stream_init(struct out_stream *s);
void out_stream_write_char(struct out_stream *s, char c);
void out_stream_write_str(struct out_stream *s, char *str);
void out_stream_flush(struct out_stream *s);
