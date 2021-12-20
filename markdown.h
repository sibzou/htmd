struct markdown_parser {
	struct out_stream out_stream;
};

void markdown_parser_init(struct markdown_parser *s, char *proc_name);
void markdown_parse(struct markdown_parser *s, struct parser_char *pch);
void markdown_parse_force(char c);
void markdown_parser_close(struct markdown_parser *s);
