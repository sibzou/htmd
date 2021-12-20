#include <stdio.h>
#include <stdlib.h>

void syscall_error(int ret, char *proc_name, char *syscall_name) {
    if(ret == -1) {
        fprintf(stderr, "%s: ", proc_name);
        perror(syscall_name);
        exit(1);
    }
}
