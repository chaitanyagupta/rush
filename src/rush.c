#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 512
#define MAX_ARGC 4

// TODO: error handling
void prompt_and_read(char *line, size_t max_len) {
    fputs("$ ", stdout);
    fgets(line, max_len, stdin);
    strsep(&line, "\n");
}

int parse_line(char *line, char **argv, size_t max_argc) {
    char **ap;
    for (ap = argv; (*ap = strsep(&line, " \t")) != NULL;) {
        if (**ap != '\0')
            if (++ap >= &argv[max_argc])
                break;
    }
    *ap = NULL;
    return ap - argv;
}

int main() {
    char line[MAX_LINE_LENGTH] = { 0 };
    char *argv[MAX_ARGC + 1];
    int should_exit;
    do {
        prompt_and_read(line, sizeof(line));
        should_exit = strcmp(line, "exit") == 0;
        if (!should_exit) {
            int argc = parse_line(line, argv, sizeof(argv));
            for (int i = 0; i < argc; ++i) {
                printf("argv[%d] = %s\n", i, argv[i]);
            }
        }
    } while (!should_exit);
    return 0;
}
