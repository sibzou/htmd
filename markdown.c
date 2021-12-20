#include <stdbool.h>

#include "htmd.h"
#include "markdown.h"

void markdown_parse(struct parser_char *pch) {
	pch->parsed = true;
	pch->move_count = 1;
}

void markdown_parse_force(char c) {

}
