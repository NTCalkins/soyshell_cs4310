#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
    if (argc < 2) {
        puts("No directory/directories given");
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        int r = rmdir(argv[i]);
        if (r == -1) {
            int r = remove(argv[i]);
            if (r == -1) {
                printf("%s could not be removed: either not empty or nonexistent\n", argv[i]);
                return 1;
            }
        }
    }
    return 0;
}
