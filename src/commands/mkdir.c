#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
int main(int argc, char **argv)
{
    int status;
    if (argc != 2)
    {
	    puts("No arguments given");
	    return 1;
    }
    status = mkdir(argv[1],0700);
    if (status == -1)
    {
        if (errno == EEXIST) {
            puts("Error: directory already exists");
        }
        return 1;
    }
    return 0;
}
