#include <stdio.h>
#include <stdlib.h>

int main (int argc, char** argv) {
	
	if (argc != 3) {
		printf("cp: invalid number of arguments\n");
		return 1;
	}

	FILE *fp_source = fopen(argv[1], "r");

	if (!fp_source) {
		printf("cp: file %s does not exist\n",argv[1]);
		return 1;
	}
	
	//We're copying over this file anyways, clean opening
	remove(argv[2]);
	FILE *fp_dest = fopen(argv[2], "w");

	char c;

	while (( c = fgetc(fp_source)) != EOF) {
		fputc(c,fp_dest);
	}
	fclose(fp_source);
	fclose(fp_dest);
	
	return 0;
}
