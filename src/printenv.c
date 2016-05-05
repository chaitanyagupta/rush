// printenv.c
#include <stdio.h>

extern char **environ;

int main() {
    char **env;
    for (env = environ; *env != NULL; ++env) {
        printf("%s\n", *env);
    }
    return 0;
}
