#include "merge.h"
#include "disk_sort.h"

void distribution(char * filename, int block_size, int column_id, int max_degree) {
	int records_per_block = block_size/sizeof(Record);
	FILE *fp_read;
	// allocate array 
	unsigned long long degree_count[max_degree + 1];

	memset(degree_count, 0 , max_degree + 1);
	
	Record * buffer = (Record *) calloc (records_per_block, sizeof(Record));
	if (!(fp_read = fopen(filename, "rb"))) {
    	printf("Error: could not open file for read.\n");
    	exit(1);
    }

   int result;
   int i;
   int id;
   int count = 0;
   int flag = 1;
	if (column_id == 1) {
		while((result = fread (buffer, sizeof(Record), records_per_block, fp_read)) > 0){
			i = 0;
			while(i < result){
				if (flag == 1) {
					id = buffer[i].UID1;
					flag = 0;
				}
				if (buffer[i].UID1 == id) {
					count++;
				} else {
					degree_count[count]++;
					id = buffer[i].UID1;
					count = 1;
				}				
				i += 1;			
			};

		};
	} else { // UID2
		while((result = fread (buffer, sizeof(Record), records_per_block, fp_read)) > 0){
			i = 0;
			while(i < result){
				if (flag == 1) {
					id = buffer[i].UID2;
					flag = 0;
				}
				if (buffer[i].UID2 == id) {
					count++;
				} else {
					degree_count[count]++;
					count = 1;
					id = buffer[i].UID2;
				}				
				i += 1;			
			};
		}
	}
	
		FILE * fp;
	if (column_id == 1) {
		fp = fopen("outdegree.csv", "w+");
	} else {
		fp = fopen("indegree.csv", "w+");
	}
	fprintf(fp,"degree, count\n");
	for (i = 1; i <= max_degree; i++) {
		if (column_id == 1) {
			printf("out degree of %d has count %llu\n", i, degree_count[i]);
		} else {
			printf("in degree of %d has count %llu\n", i, degree_count[i]);
		}
		fprintf(fp, "%d,%llu\n", i, degree_count[i]); //Write the data to a temporary file
		
	}

   fclose(fp);
	free(buffer);
   fclose(fp_read);
	//return degree_count;
}

int compute_max_in_degree(char *filename, int block_size) {
	int records_per_block = block_size/sizeof(Record);
	FILE *fp_read;

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

	while((result = fread (buffer, sizeof(Record), records_per_block, fp_read)) > 0){
		int i = 0;
		total_records++;
		
		if (result != records_per_block){
			records_per_block = result;
		};

		while(i < records_per_block){
			total_edges += 1;
			if (current_id == -1){
				current_id = buffer[i].UID2;
				current_followers = 1;
				unique_users = 1;
			}else if (current_id == buffer[i].UID2){
				current_followers += 1;
			} else {
				current_id = buffer[i].UID2;
				if (max_followers < current_followers){
					max_followers = current_followers;
				};
				current_id = buffer[i].UID2;
				current_followers = 1;
				unique_users += 1;
			};
			i += 1;
			
		};

	};
	fclose(fp_read);
	free(buffer);
	return max_followers;
}

int compute_max_out_degree(char *filename, int block_size) {
	int records_per_block = block_size/sizeof(Record);
	FILE *fp_read;

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
	while((result = fread (buffer, sizeof(Record), records_per_block, fp_read)) > 0){
		int i = 0;
		total_records++;
		
		if (result != records_per_block){
			records_per_block = result;
		};

		while(i < records_per_block){
			total_edges += 1;
			if (current_id == -1){
				current_id = buffer[i].UID1;
				current_followers = 1;
				unique_users = 1;
			}else if (current_id == buffer[i].UID1){
				current_followers += 1;
			} else {
				current_id = buffer[i].UID1;
				if (max_followers < current_followers){
					max_followers = current_followers;
				};
				current_id = buffer[i].UID1;
				current_followers = 1;
				unique_users += 1;
			};
			i += 1;
			
		};

	};
	fclose(fp_read);
	free(buffer);
	return max_followers;
}

int main(int argc, char *argv[]){
	char *filename = argv[1];
	int block_size = atoi(argv[2]);
	int column_id = atoi(argv[3]);
	int max_degree;
	if (column_id == 1) {
		max_degree = compute_max_out_degree(filename, block_size);
	} else {
		max_degree = compute_max_in_degree(filename, block_size);
	}
	distribution(filename, block_size, column_id, max_degree);
	return 0;
}




