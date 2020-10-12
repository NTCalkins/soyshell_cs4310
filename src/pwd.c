#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argv, char **argc)
{
	char cwd[1024];
	printf("Working directory: %s\n", getcwd(cwd, sizeof(cwd)));

	return 0;
}
