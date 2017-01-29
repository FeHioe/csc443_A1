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
		printf("Error: sizing issue.\n");
		exit(1);
	}

	printf("rpb: %d\n", records_per_block);

	int i = 0;
	int current_id = buffer[i].uid1;
	int current_followers = 1;
	int max_followers = 0;
	int unique_ids = 1;
	int total_edges = 1;
	//printf("current id: %d\n", current_id);
	while(i < records_per_block-1){
		i += 1;
		if (current_id == buffer[i].uid1){
			current_followers += 1;
			total_edges += 1;
		} else {
			printf("current id: %d\n", current_id);
			current_id = buffer[i].uid1;
			unique_ids += 1;
			printf("current id: %d\n", current_id);
			if (max_followers < current_followers){
				max_followers = current_followers;
			};
			current_followers = 1;
			total_edges += 1;
		};
	};

	printf("max followers: %d\n", max_followers);
	printf("unique ids: %d total egdes: %d\n", unique_ids, total_edges);
	printf("Average: %d\n", total_edges/unique_ids);
	fclose(fp_read);
	free(buffer);
	
}
