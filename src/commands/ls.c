#include <stdio.h>
#include <dirent.h>

int main(int argc, char** argv) {
	
	DIR *d;
	struct dirent *dir;
	if (argc < 2) {
		d = opendir(".");
	}
	else {
		d = opendir(argv[1]);
	}

	if (d == NULL) {
		printf("directory cannot be read.\n");
		return 1;
	}
        dir = readdir(d);
	while (dir != NULL) {
		printf("%s\n", dir->d_name);
                dir = readdir(d);
	}
	closedir(d);
	return 0;
}
