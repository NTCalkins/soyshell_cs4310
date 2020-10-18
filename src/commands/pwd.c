#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

int main(int argv, char **argc)
{
	char *cwd = NULL;
        cwd = getcwd(cwd, PATH_MAX);
        puts(cwd);
        free(cwd);
	return 0;
}
