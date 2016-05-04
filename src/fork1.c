#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/errno.h>
#include <stdlib.h>
#include <libgen.h>

#define print_error(errnum)                                             \
    fprintf(stderr, "%s:%d %s\n", __FILE__, __LINE__, strerror(errnum))

int main (int argc, char **argv) {
    pid_t (*forkptr)(void);
    char *path = argv[0];
    char *name = basename(path);
    if (strcmp("fork1", name) == 0) {
        printf("Using fork\n");
        forkptr = fork;
    } else if (strcmp("vfork1", name) == 0) {
        printf("Using vfork\n");
        forkptr = vfork;
    } else {
        fprintf(stderr, "Neither fork1 nor vfork1\n");
        exit(1);
    }
    sleep(5);
    pid_t pid = forkptr();
    if (pid > 0) {
        // in parent process, success
        printf("Successfully forked\n");
    } else if (pid < 0) {
        // in parent process, failure (no child process created)
        print_error(errno);
    } else {
        // in child process
        printf("Inside child\n");
        char *child = argv[1];
        if (execve(child, (char *[]){ child, NULL }, NULL) == -1) {
            print_error(errno);
            _exit(1);
        }
    }
    printf("Sleeping again\n");
    sleep(5);
    printf("Done sleeping\n");
    return 0;
}
