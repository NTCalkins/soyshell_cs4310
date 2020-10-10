#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argv, char **argc)
{
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	printf("Working directory: %s\n", cwd);

	return 0;
}
