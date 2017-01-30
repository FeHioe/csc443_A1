#include "utils.h"

int main(int argc, char *argv[]){
	char *filename = argv[1];
	int block_size = atoi(argv[2]);
	int x = atoi(argv[3]);

	FILE *fp_write;
	if (!(fp_write = fopen(filename, "rb+"))) {
    	printf("Error: could not open file for write.\n");
    	exit(1);
    };

	int records_in_block = block_size/sizeof(Record); 

	Record * buffer = (Record *) calloc (1, sizeof(Record));
	int i = 0;
	while (i < x) {
		int random = rand() % records_in_block;

		printf("random: %d\n", random);
		i += 1;

		fseek(fp_write, random*sizeof(Record), SEEK_SET);
		buffer[0].uid1 = 1;
		buffer[0].uid2 = 2;
		fwrite(buffer, sizeof(Record), 1, fp_write);
		fflush(fp_write);
	};
	
	return 0;
}
