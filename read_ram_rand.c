#include "utils.h"

int main(int argc, char *argv[]){
	char *filename = argv[1];
	int block_size = atoi(argv[2]);
	int x = atoi(argv[3]);

   struct timeb t_begin, t_end;
   long time_spent_ms;
   long total_records = 0;

	int records_per_block = block_size/sizeof(Record);
	FILE *fp_read;
	if (!(fp_read = fopen(filename, "rb"))) {
    	printf("Error: could not open file for read.\n");
    	exit(1);
    }
	
	fseek(fp_read, 0, SEEK_END);
	int file_size = ftell(fp_read);
	fseek(fp_read, 0, SEEK_SET);
	int records_in_file = file_size/sizeof(Record); 

	Record * buffer = (Record *) calloc (records_in_file, sizeof(Record));

	int result = fread(buffer, sizeof(Record), records_in_file, fp_read);
	printf("result: %d\n", result);
	if (result != records_in_file){
		printf("Error: sizing issue.\n");
		exit(1);
	}

	int i = 0;
	ftime(&t_begin);
	//printf("current id: %d\n", current_id);
	while(i < x){
		i += 1;

		int random = rand() % (file_size/block_size);
		int j = random * records_per_block;
		int k = j + records_per_block;

		int current_id = buffer[j].uid1;
		int current_followers = 0;
		int max_followers = 0;
		int unique_ids = 1;
		int total_edges = 0;

		while (j < k){
			total_records++;
			//printf("j: %d\n", j);
			printf("uid1 %d   uid2 %d\n", buffer[j].uid1, buffer[j].uid2);
			if (current_id == buffer[j].uid1){
				current_followers += 1;
				total_edges += 1;
				if (max_followers < current_followers){
					max_followers = current_followers;
				};
			} else {
				//printf("current id: %d\n", current_id);
				current_id = buffer[j].uid1;
				unique_ids += 1;
				//printf("current id: %d\n", current_id);
				if (max_followers < current_followers){
					max_followers = current_followers;
				};
				current_followers = 1;
				total_edges += 1;
			};
			j +=1;
		};
		printf("iteration #%d\n", j);
		printf ("Total Records: %d unique users: %d\n", total_edges, unique_ids);
		printf("max followers: %d\n\n", max_followers);
	};

	fclose(fp_read);
	ftime(&t_end);
	free(buffer);

	time_spent_ms = (long) (1000 *(t_end.time - t_begin.time)
       + (t_end.millitm - t_begin.millitm)); 
   printf ("Data rate: %.3f MBPS\n", ((total_records*sizeof(Record))/(float)time_spent_ms * 1000)/1000000);	

	return 0;
}
