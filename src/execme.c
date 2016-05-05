// execme.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_VAR_LEN 2048
#define MAX_PATH_LEN 512

extern char **environ;

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

int main(int argc, char *argv[]) {
    char path[MAX_PATH_LEN];
    if (!search_path(argv[1], path, sizeof(path))) {
        fprintf(stderr, "Couldn't locate %s\n", argv[1]);
        return -1;
    }
    int pargc = argc - 1;
    char **pargv = malloc(sizeof(char *) * (pargc + 1));
    for (int i = 0; i < pargc; ++i) {
        pargv[i] = argv[i + 1];
    }
    pargv[argc] = NULL;
    if (execve(path, pargv, environ) == -1) {
        perror(__FILE__);
    }
    return -1;
}
