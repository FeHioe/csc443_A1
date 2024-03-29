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


	int result = 0;
	int current_id = -1;
	int current_followers = 0;
	int max_followers = 0;
	int total_edges = 0;
    int unique_users = 0;
    ftime(&t_begin); 
	while((result = fread (buffer, sizeof(Record), records_per_block, fp_read)) > 0){
		int i = 0;
		total_records++;
		
		if (result != records_per_block){
			records_per_block = result;
		};

		while(i < records_per_block){
			total_edges += 1;
			//printf("current: %d\n", current_id);
			//printf("uid1 %d   uid2 %d\n", buffer[i].uid1, buffer[i].uid2);
			
			if (current_id == -1){
				current_id = buffer[i].uid1;
				current_followers = 1;
				unique_users = 1;
			}else if (current_id == buffer[i].uid1){
				current_followers += 1;
			} else {
				current_id = buffer[i].uid1;
				if (max_followers < current_followers){
					max_followers = current_followers;
				};
				current_id = buffer[i].uid1;
				current_followers = 1;
				unique_users += 1;
			};
			i += 1;
			
		};

	};

	printf ("Total Records: %d \nUnique Users: %d \nAverage: %d\n", total_edges, unique_users, total_edges/unique_users);
	printf("Max Followers: %d\n", max_followers);
	fclose(fp_read);
	ftime(&t_end);
	free(buffer);
	
	time_spent_ms = (long) (1000 *(t_end.time - t_begin.time)
       + (t_end.millitm - t_begin.millitm)); 
   printf ("Data rate: %.3f MBPS\n", ((total_records*sizeof(Record))/(float)time_spent_ms * 1000)/(1024*1024));	
	
	return 0;
}
