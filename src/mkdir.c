#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
int main(int argc, char **argv)
{
    int status;
    printf("%i",argc);
    if (argc != 2)
    {
	    printf("No arguments given");
	    return 0;
    }
    printf(argv[1]);
    status = mkdir(argv[1],0700);
    return 0;
}
