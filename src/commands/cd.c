#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{

    if (argc < 2)
    {
        printf("Not enough arguments\n");
        return 0;
    }
    else if (argc > 3) {
        printf("Too many arguments\n");
        return 0;
    }

    char cwd[1024];

    // Changes directory still? I think?
    if (chdir(argv[1]) == -1) {
        perror("Failed to change directory.");
        return -1;
    }

    return 0;
}
