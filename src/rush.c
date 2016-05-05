#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 512
#define MAX_ARGC 64

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

#define ARRAY_SIZE(x)  (sizeof(x) / sizeof((x)[0]))

int main() {
    char line[MAX_LINE_LENGTH];
    char *argv[MAX_ARGC];
    int should_exit;
    do {
        prompt_and_read(line, sizeof(line));
        should_exit = strcmp(line, "exit") == 0;
        if (!should_exit) {
            int argc = parse_line(line, argv, ARRAY_SIZE(argv));
            for (int i = 0; i < argc; ++i) {
                printf("argv[%d] = %s\n", i, argv[i]);
            }
        }
    } while (!should_exit);
    return 0;
}
