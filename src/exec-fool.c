#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/errno.h>

#define print_error(errnum)                                             \
    fprintf(stderr, "%s:%d %s\n", __FILE__, __LINE__, strerror(errnum))

int main (int argc, char *argv[]) {
    char *program = argv[1];
    if (execve(program, &argv[2], NULL) == -1) {
        print_error(errno);
    }
    return 0;
}
