#include "utils.h"

int main(int argc, char *argv[]){
	char *filename = argv[1];
	int block_size = atoi(argv[2]);
	int records_per_block = block_size/sizeof(Record);
	FILE *fp_read;

	Record * buffer = (Record *) calloc (records_per_block, sizeof(Record));
	if (!(fp_read = fopen(filename, "rb"))) {
    	printf("Error: could not open file for read.");
    	exit(1);
    }

	int result = fread(buffer, sizeof(Record), records_per_block, fp_read);
	if (result != records_per_block){
		printf("Error: sizing issue.");
		exit(1);
	}

	printf("rpb: %d\n", records_per_block);

	int i = 0;
	while (i < 27){
		i += 1;
		printf("uid1: %d\n", buffer[i].uid1);
	};

	fclose(fp_read);
	free(buffer);
	
}
