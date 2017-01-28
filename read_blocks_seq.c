#include "utils.h"

int main(int argc, char *argv[]){
	char *filename = argv[1];
	int block_size = atoi(argv[2]);
	int records_per_block = block_size/sizeof(Record);
	FILE *fp_read;

	Record * buffer = (Record *) calloc (records_per_block, sizeof(Record));
	if (!(fp_read = fopen(filename, "rb"))) {
    	printf("Error: could not open file for read.\n");
    	exit(1);
    }

	int result = fread(buffer, sizeof(Record), records_per_block, fp_read);
	if (result != records_per_block){
		printf("Error: sizing issue.");
		exit(1);
	}

	printf("rpb: %d\n", records_per_block);

	int i = 0;
	int current_id = buffer[i].uid1;
	int current_followers = 1;
	int max_followers = 0;
	//printf("current id: %d\n", current_id);
	
	

	while(i < records_per_block){
		i += 1;
		if (current_id == buffer[i].uid1){
			current_followers += 1;
		} else {
			printf("current id: %d\n", current_id);
			current_id = buffer[i].uid1;
			if (current_followers > max_followers){
				max_followers = current_followers;
			}
		};
	};

	printf("max followers: %d\n", max_followers);
	fclose(fp_read);
	free(buffer);
	
}
