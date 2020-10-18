#include <unistd.h>
#include <stdio.h>

int main (int argc, char **argv)
{
	if (argc != 2) {
		printf("No directory given");
		return 0;
	}
	int r = rmdir(argv[1]);
	if (r == 0) {
		printf(argv[1]);
		printf(" removed\n");
	}
	else {
		printf(argv[1]);
		printf(" could not be removed: either not empty or nonexistent\n");
	}
	return 0;
}
