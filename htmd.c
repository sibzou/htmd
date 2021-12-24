#include <unistd.h>
#include <stdbool.h>
#include <err.h>
#include <stdlib.h>

#include "htmd.h"
#include "paragraph.h"
#include "misc.h"
#include "outstream.h"
#include "markdown.h"

int main(int argc, char *argv[]) {
    int in_fd = STDIN_FILENO;

    char in_buf[BUFFER_SIZE];
    int in_buf_len = 0;

    int in_buf_unparse_start = BUFFER_SIZE;

    char backlog[BUFFER_SIZE];
    int backlog_start = 0;
    int backlog_len = 0;

    int user_cursor = 0;

    struct markdown_parser mdp;
    markdown_parser_init(&mdp);

    while(true) {
        struct parser_char pch;

        // if we are out of saved data, we continue to read the input stream
        if(user_cursor >= backlog_len + in_buf_len) {
            user_cursor -= in_buf_unparse_start > in_buf_len ? in_buf_len : in_buf_unparse_start;

            // copying unparsed data from the read buffer to the backlog
            for(int i = in_buf_unparse_start; i < in_buf_len; i++) {
                if(backlog_len < BUFFER_SIZE) {
                    backlog[(backlog_start + backlog_len) % BUFFER_SIZE] = in_buf[i];
                    backlog_len++;
                } else {
                    // we force parsing if there is no more space in the backlog
                    markdown_parse_force(&mdp, backlog[backlog_start]);
                    backlog[backlog_start] = in_buf[i];
                    backlog_start = (backlog_start + 1) % BUFFER_SIZE;

                    // after forced parsing, we start parse again from the first saved data
                    user_cursor = 0;
                }
            }

            in_buf_len = read(in_fd, in_buf, BUFFER_SIZE);

            if(in_buf_len == -1) {
                err(EXIT_FAILURE, "can't read the input");
            }
        }

        // get the real character from user_cursor
        if(user_cursor < backlog_len) {
            pch.c = backlog[(backlog_start + user_cursor) % BUFFER_SIZE];
            pch.end = false;
        } else if(user_cursor < backlog_len + in_buf_len) {
            pch.c = in_buf[user_cursor - backlog_len];
            pch.end = false;
        } else {
            pch.end = true;
        }

        markdown_parse(&mdp, &pch);

        if(pch.end && pch.move_count >= 0) {
            break;
        } else if(user_cursor < backlog_len) {
            if(pch.parsed) {
                backlog_start = (backlog_start + user_cursor + 1) % BUFFER_SIZE;
                backlog_len -= user_cursor + 1;
                user_cursor = -1;
            }
        } else if(user_cursor < backlog_len + in_buf_len) {
            if(pch.parsed) {
                user_cursor -= backlog_len;
                backlog_len = 0;
                in_buf_unparse_start = BUFFER_SIZE;
            } else if(in_buf_unparse_start == BUFFER_SIZE) {
                in_buf_unparse_start = user_cursor - backlog_len;
            }
        }

        user_cursor += pch.move_count;
    }
}
