#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
int main(int argc, char **argv)
{
    int status;
    int r = 0;
    if (argc < 2)
    {
	    puts("No arguments given");
	    return 1;
    }
    for (int i = 1; i < argc; i++) {
        status = mkdir(argv[i],0700);
        if (status == -1)
        {
            if (errno == EEXIST) {
                printf("Error: directory %s already exists\n",argv[i]);
            }
            r = 1;
        }
    }
    return r;
}
