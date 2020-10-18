#include <unistd.h>
#include <stdio.h>

int main (int argc, char **argv)
{
    if (argc != 2) {
        puts("No directory given");
        return 0;
    }
    int r = rmdir(argv[1]);
    if (r == -1) {
        printf("%s could not be removed: either not empty or nonexistent\n", argv[1]);
        return 1;
    }
    return 0;
}
