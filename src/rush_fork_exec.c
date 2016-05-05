#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define ARRAY_SIZE(x)  (sizeof(x) / sizeof((x)[0]))

#define MAX_LINE_LENGTH 512
#define MAX_ARGC 64
#define MAX_PATH_VAR_LEN 2048
#define MAX_PATH_LEN 512

// TODO: error handling
void prompt_and_read(char *line, size_t max_len) {
    fputs("$ ", stdout);
    fgets(line, max_len, stdin);
    strsep(&line, "\n");
}

int parse_line(char *line, char *argv[], size_t max_argc) {
    char **ap = argv;
    while ((*ap = strsep(&line, " \t")) != NULL) {
        if (**ap != '\0')
            if (++ap >= &argv[max_argc - 1])
                break;
    }
    *ap = NULL;
    return ap - argv;
}

int search_path(char *name, char *path, size_t maxlen) {
    char pathdirs[MAX_PATH_VAR_LEN];
    strlcpy(pathdirs, getenv("PATH"), sizeof(pathdirs));
    char *dir, *head = pathdirs;
    int success = 0;
    while ((dir = strsep(&head, ":")) != NULL) {
        strlcpy(path, dir, maxlen);
        strlcat(path, "/", maxlen);
        strlcat(path, name, maxlen);
        if (access(path, X_OK) == 0) {
            return 1;
        }
    }
    return 0;
}

extern char **environ;

int main() {
    char line[MAX_LINE_LENGTH];
    char *argv[MAX_ARGC];
    int should_exit;
    do {
        prompt_and_read(line, sizeof(line));
        should_exit = strcmp(line, "exit") == 0;
        if (!should_exit) {
            int argc = parse_line(line, argv, ARRAY_SIZE(argv));
            char binpath[MAX_PATH_LEN];
            if (search_path(argv[0], binpath, sizeof(binpath))) {
                pid_t pid = fork();
                if (pid == 0) {
                    if (execve(binpath, argv, environ) == -1) {
                        perror("rush");
                    }
                } else {
                    // do something else
                }
            } else {
                fprintf(stderr, "Couldn't locate %s\n", argv[0]);
            }
        }
    } while (!should_exit);
    return 0;
}
