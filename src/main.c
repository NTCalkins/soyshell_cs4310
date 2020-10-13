#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

char *read_command(void) {
    char *command = NULL;
    ssize_t n = 0;

    if (getline(&command, &n, stdin) == -1) {
        return command;
    }
    return command;
}

int main(int argc, char** argv) {
    char d[1024];
    if (getcwd(d, sizeof(d)) != NULL) {
        //TODO
    }
    char *command;
    do {
        printf("%s> ", d);
        command = read_command();
        printf("Command entered: %s", command);
        printf("%i",strcmp(command, "exit\n"));
    } while (strcmp(command,"exit\n") != 0);

    return 0;
}