// forkme.c
#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();
    if (pid > 0) {
        printf("In parent process, child pid: %d\n", pid);
    } else if (pid == 0) {
        printf("In child process\n");
    } else {
        perror(__FILE__);
        return -1;
    }
    sleep(10);
/*
Output of `ps -eaf`

  UID   PID  PPID   C STIME   TTY           TIME CMD
  501 85638 78129   0 12:27PM ttys001    0:00.00 bin/forkme
  501 85639 85638   0 12:27PM ttys001    0:00.00 bin/forkme
*/
    return 0;
}
