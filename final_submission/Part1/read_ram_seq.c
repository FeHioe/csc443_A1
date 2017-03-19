#include "utils.h"

int main(int argc, char *argv[]){
	char *filename = argv[1];
	int block_size = atoi(argv[2]);
	int records_per_block = block_size/sizeof(Record);
	FILE *fp_read;

   struct timeb t_begin, t_end;
   long time_spent_ms;
   long total_records = 0;

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

	//printf("rpb: %d\n", records_per_block);

	int i = 0;
	int current_id = buffer[i].uid1;
	int current_followers = 1;
	int max_followers = 0;
	int unique_ids = 1;
	int total_edges = 1;
	//printf("current id: %d\n", current_id);
	ftime(&t_begin);
	while(i < records_per_block-1){
		total_records++;
		i += 1;
		//printf("uid1: %d uid2: %d\n", buffer[i].uid1, buffer[i].uid2);
		if (current_id == buffer[i].uid1){
			current_followers += 1;
			total_edges += 1;
		} else {
			//printf("current id: %d\n", current_id);
			current_id = buffer[i].uid1;
			unique_ids += 1;
			//printf("current id: %d\n", current_id);
			if (max_followers < current_followers){
				max_followers = current_followers;
			};
			current_followers = 1;
			total_edges += 1;
		};
	};

	printf ("Total Records: %d \nUnique Users: %d \nAverage: %d\n", total_edges, unique_ids, total_edges/unique_ids);
	printf("Max Followers: %d\n", max_followers);
	fclose(fp_read);
	ftime(&t_end);
	free(buffer);
	
  time_spent_ms = (long) (1000 *(t_end.time - t_begin.time) + (t_end.millitm - t_begin.millitm)); 
  printf ("Data rate: %.3f MBPS\n", ((total_records*sizeof(Record))/(float)time_spent_ms * 1000)/(1024*1024));

	return 0;
}
