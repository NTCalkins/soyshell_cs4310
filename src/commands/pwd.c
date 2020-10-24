#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

int main(int argc, char **argv)
{
	char *cwd = NULL;
        if (argc != 1 ) {
                puts("pwd: too many arguments");
                return -1;
        }
        cwd = getcwd(cwd, PATH_MAX);
        puts(cwd);
        free(cwd);
	return 0;
}
