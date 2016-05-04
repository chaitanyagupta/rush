#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/errno.h>
#include <stdlib.h>

#define print_error(errnum)                                             \
    fprintf(stderr, "%s:%d %s\n", __FILE__, __LINE__, strerror(errnum))

int main (int argc, char **argv) {
    printf("In %s\n", argv[0]);
    sleep(5);
    pid_t pid = vfork();
    if (pid > 0) {
        // in parent process, success
        printf("Successfully vforked\n");
    } else if (pid < 0) {
        // in parent process, failure (no child process created)
        print_error(errno);
    } else {
        // in child process
        printf("Inside child\n");
        sleep(5);
        char *program = argv[1];
        if (execve(program, (char *[]){ program, NULL }, NULL) == -1) {
            print_error(errno);
            exit(1);
        }
    }
    return 0;
}
