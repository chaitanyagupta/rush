// execprintargs.c
#include <stdio.h>
#include <unistd.h>

extern char **environ;

int main() {
    char *path = "bin/printargs";
    char *argv[] = {"printargs", "foo", "bar", NULL};
/*
$ ps -eaf
  UID   PID  PPID   C STIME   TTY           TIME CMD
  501 86779 73189   0  1:20PM ttys000    0:00.00 execprintargs
*/
    if (execve(path, argv, environ) == -1) {
        perror("execprintargs");
    }
/*
$ ps -eaf
  UID   PID  PPID   C STIME   TTY           TIME CMD
  501 86779 73189   0  1:20PM ttys000    0:00.01 printargs foo bar
*/
    return -1;
}
