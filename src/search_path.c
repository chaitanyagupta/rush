#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PATH_VAR_LEN 2048
#define MAX_PATH_LEN 512

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

int main(int argc, char *argv[1]) {
    char path[MAX_PATH_LEN];
    if (search_path(argv[1], path, sizeof(path))) {
        puts(path);
        return 0;
    } else {
        return -1;
    }
}
