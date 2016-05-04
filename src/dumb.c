#include <stdio.h>
#include <unistd.h>

int main (int argc, char **argv) {
    printf("In %s\n", argv[0]);
    sleep(5);
    printf("Finished %s\n", argv[0]);
    return 0;
}
