#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

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

// Check argv for redirect arguments (starting with '<' or '>').
//
// If present, they should be at the end of argv, and `input` and
// `output` will point to them after the function exits. `argv` is
// also truncated to exclude redirect arguments.
int get_redirect_files(int argc, char **argv, char **input, char **output) {
    for (int i = argc - 1; i >= 0; --i) {
        char *arg = argv[i];
        if (arg[0] == '<') {
            *input = arg + 1;
            --argc;
        } else if (arg[0] == '>') {
            *output = arg + 1;
            --argc;
        } else {
            break;
        }
    }
    argv[argc] = NULL;
    return argc;
}

// If `inpath` is not NULL, redirect standard input
// If `outpath` is not NULL, redirect standard output
void redirect_std(char *inpath, char *outpath) {
    if (inpath != NULL) {
        int in = open(inpath, O_RDONLY);
        if (in == -1) {
            perror("rush (open)");
        } else if (dup2(in, STDIN_FILENO) == -1) {
            perror("rush (dup2)");
        }
    }
    if (outpath != NULL) {
        int out = open(outpath, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        if (out == -1) {
            perror("rush (open)");
        } else if (dup2(out, STDOUT_FILENO) == -1) {
            perror("rush (dup2)");
        }
    }
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
                if (pid > 0) {
                    wait(NULL);
                } else if (pid == 0) {
                    char *input, *output;
                    input = output = NULL;
                    if (get_redirect_files(argc, argv, &input, &output) < argc) {
                        redirect_std(input, output);
                    }
                    if (execve(binpath, argv, environ) == -1) {
                        perror("rush");
                    }
                } else {
                    perror("rush");
                }
            } else {
                fprintf(stderr, "Couldn't locate %s\n", argv[0]);
            }
        }
    } while (!should_exit);
    return 0;
}
