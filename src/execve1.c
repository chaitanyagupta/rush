#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/errno.h>

#define print_error(errnum)                                             \
    fprintf(stderr, "%s:%d %s\n", __FILE__, __LINE__, strerror(errnum))

int main (int argc, char **argv) {
    printf("In %s\n", argv[0]);
    sleep(5);
    char *program = "bin/dumb";
    if (execve(program, (char *[]){ program, NULL }, NULL) == -1) {
        print_error(errno);
    }
    return 0;
}
