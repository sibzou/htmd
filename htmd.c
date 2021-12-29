#include <unistd.h>
#include <stdbool.h>
#include <err.h>
#include <stdlib.h>

#include "misc.h"
#include "htmd.h"
#include "paragraph.h"
#include "outstream.h"
#include "markdown.h"

static void input_buffer_init(struct input_buffer *s) {
    s->read_buf_len = 0;
    s->read_buf_unparse_start = BUFFER_SIZE;
    s->backlog_start = 0;
    s->backlog_len = 0;
    s->user_cursor = 0;
}

static void read_fd_if_needed(struct input_buffer *s, int read_fd, struct markdown_parser *mdp) {
    // if we are out of saved data, we continue to read the input stream
    if(s->user_cursor >= s->backlog_len + s->read_buf_len) {
        if(s->read_buf_unparse_start == BUFFER_SIZE) {
            s->user_cursor = 0;
        } else {
            s->user_cursor -= s->read_buf_unparse_start;
        }

        // copying unparsed data from the read buffer to the backlog
        for(int i = s->read_buf_unparse_start; i < s->read_buf_len; i++) {
            if(s->backlog_len < BUFFER_SIZE) {
                s->backlog[(s->backlog_start + s->backlog_len) % BUFFER_SIZE] = s->read_buf[i];
                s->backlog_len++;
            } else {
                // we force parsing if there is no more space in the backlog
                markdown_parse_force(mdp, s->backlog[s->backlog_start]);
                s->backlog[s->backlog_start] = s->read_buf[i];
                s->backlog_start = (s->backlog_start + 1) % BUFFER_SIZE;

                // after forced parsing, we start parse again from the first saved data
                s->user_cursor = 0;
            }
        }

        s->read_buf_len = read(read_fd, s->read_buf, BUFFER_SIZE);

        if(s->read_buf_len == -1) {
            err(EXIT_FAILURE, "can't read the input");
        }

        s->read_buf_unparse_start = BUFFER_SIZE;
    }
}

// get the real character from user_cursor
static void get_user_cursor_char(struct input_buffer *s, struct parser_char *pch) {
    if(s->user_cursor < s->backlog_len) {
        pch->c = s->backlog[(s->backlog_start + s->user_cursor) % BUFFER_SIZE];
        pch->end = false;
    } else if(s->user_cursor < s->backlog_len + s->read_buf_len) {
        pch->c = s->read_buf[s->user_cursor - s->backlog_len];
        pch->end = false;
    } else {
        pch->end = true;
    }
}

static bool handle_parse_result(struct input_buffer *s, struct parser_char *pch) {
    if(pch->end && pch->move_count >= 0) {
        return true;
    } else if(s->user_cursor < s->backlog_len) {
        if(pch->parsed) {
            s->backlog_start = (s->backlog_start + s->user_cursor + 1) % BUFFER_SIZE;
            s->backlog_len -= s->user_cursor + 1;
            s->user_cursor = -1;
        }
    } else if(s->user_cursor < s->backlog_len + s->read_buf_len) {
        if(pch->parsed) {
            s->user_cursor -= s->backlog_len;
            s->backlog_len = 0;
            s->read_buf_unparse_start = BUFFER_SIZE;
        } else if(s->read_buf_unparse_start == BUFFER_SIZE) {
            s->read_buf_unparse_start = s->user_cursor - s->backlog_len;
        }
    }

    s->user_cursor += pch->move_count;
    return false;
}

int main(int argc, char *argv[]) {
    int read_fd = STDIN_FILENO;
    struct input_buffer input_buffer;
    struct markdown_parser mdp;

    input_buffer_init(&input_buffer);
    markdown_parser_init(&mdp);

    while(true) {
        struct parser_char pch;

        read_fd_if_needed(&input_buffer, read_fd, &mdp);
        get_user_cursor_char(&input_buffer, &pch);

        markdown_parse(&mdp, &pch);

        if(handle_parse_result(&input_buffer, &pch)) {
            break;
        }
    }
}
