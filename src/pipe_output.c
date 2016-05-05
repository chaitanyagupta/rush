#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

extern char **environ;

int main(int argc, char *argv[]) {
    char *outpath = argv[1];
    int pargc = argc - 2;
    char **pargv = malloc(sizeof(char *) * (pargc + 1));
    for (int i = 0; i < pargc; ++i) {
        pargv[i] = argv[i + 2];
    }
    pargv[argc] = NULL;
    pid_t pid = fork();
    if (pid > 0) {
        printf("In parent process\n");
    } else if (pid == 0) {
        int out = open(outpath, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        if (out == -1) {
            perror(__FILE__);
            return -1;
        }
        if (dup2(out, STDOUT_FILENO) == -1) {
            perror(__FILE__);
            return -1;
        }
        printf("In child process\n");
        fflush(stdout);
        if (execve(pargv[0], pargv, environ) == -1) {
            perror("pipe_output");
        }
    } else {
        perror("pipe_output");
    }
    return -1;
}
