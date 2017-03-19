#include "merge.h"
#include "celebrities.h"
#include <math.h>
#include <sys/timeb.h>

int sort_file (char *filename, int total_mem, int block_size, int id );

int get_degrees(char* file_name, int mem_size, int block_size, int id){	
	// Open given file for reading
	FILE *fp_read;
    if (!(fp_read = fopen (file_name , "rb"))){
		printf("Error opening file: %s\n", file_name);
		exit(1);
	}

	// Open output file for writing
	char *degree_file;
	if (id == 1){ // Read the first coloumn
		degree_file = "outdegree.dat";
	}
	else{ // Read the second coloumn
		degree_file = "indegree.dat";
	}
	
	FILE *fp_write;
	if (!(fp_write = fopen (degree_file , "wb"))){
		printf("Error opening file: %s\n", degree_file);
		exit(1);
	}

	// Get total file size
    fseek(fp_read, 0, SEEK_END);
    int filesize = ftell(fp_read);
    fseek(fp_read, 0, SEEK_SET); 

    // Partition into K chunks of maximum possible size
    int k = ceil((float)filesize / mem_size);
    printf("filesize: %d mem_size: %d k: %d\n", filesize, mem_size, k);

    // Determine chunk size
    int chunk_size = ceil((float)filesize / k);
    printf("chunk size: %d\n", chunk_size);

    Record *input_buffer = (Record*) calloc (chunk_size, sizeof(Record));
    Degree *count_buffer = (Degree*) calloc (block_size/sizeof(Degree), sizeof(Degree));
    int count_i = 0;
    int current_uid = -1;
    int count = 0;

    int i;
    for (i=0 ; i < k; i ++){
      // Align chunk with block size 
      Record *block_buffer = (Record*) calloc (block_size, sizeof(Record));

      int num_block = 0;
      int block_elements = block_size / sizeof(Record);
      int test = ceil(((float)chunk_size / block_size));
      int buffer_i = 0;

      while (num_block < test){
        if (fread(block_buffer, sizeof(Record), block_elements, fp_read) == 0){ // Read a block
        	printf("Read Error\n");
        };

        // Read into block buffer
        int j;
        for (j=0; j < block_elements; j++) {
          input_buffer[buffer_i].UID1 = block_buffer[j].UID1;
          input_buffer[buffer_i].UID2 = block_buffer[j].UID2;
          buffer_i++;
        };
        
        // Check for non-full block to read into block buffer
        if ((num_block+1 == test) && (chunk_size % block_size != 0)){
          block_elements = (chunk_size % block_size) / sizeof(Record);
          if (fread(block_buffer, sizeof(Record), block_elements, fp_read) == 0 ){
        	printf("Read Error\n");
          };

          int j;
          for (j=0; j < block_elements; j++) {
            input_buffer[buffer_i].UID1 = block_buffer[j].UID1;
            input_buffer[buffer_i].UID2 = block_buffer[j].UID2;
            buffer_i++;
          };
        };
        num_block++;

      };

      int y;
      for (y=0; y < test; y++){
      	if (id == 1) { // outdegree
	      	if (current_uid == -1){
	      		// Initialize
	      		current_uid = input_buffer[0].UID1;
	      		count++;

	      		// printf("init\n");
	      		// printf ("uid1: %d count: %d\n", current_uid, count);

	      	} else if (current_uid == input_buffer[y].UID1) {
	      		count++;
	      		// printf("add\n");
	      		// printf ("uid1: %d count: %d\n", current_uid, count);
	      	} else {
	      		// Uid1 changed
	      		count_buffer[count_i].UID = current_uid;
	      		count_buffer[count_i].count = count;

	      		// printf ("buffer uid1: %d count: %d\n", count_buffer[count_i].UID, count_buffer[count_i].count);

				count_i++;
				// printf("switch\n");
				// printf ("uid1: %d count: %d\n", current_uid, count);


				if (block_size/sizeof(Degree) <= count_i){
					// printf("switch\n");
					fwrite (count_buffer, sizeof(Degree), count_i, fp_write);
					count_i = 0;
				}

				current_uid = input_buffer[y].UID1;
				count = 1;
	      	}
	    } else { // indegree


	      	if (current_uid == -1){
	      		// Initialize
	      		current_uid = input_buffer[0].UID2;
	      		count++;

	      		// printf("init\n");
	      		// printf ("uid2: %d count: %d\n", current_uid, count);

	      	} else if (current_uid == input_buffer[y].UID2) {
	      		count++;
	      		// printf("add\n");
	      		// printf ("uid2: %d count: %d\n", current_uid, count);
	      	} else {
	      		// Uid1 changed
	      		count_buffer[count_i].UID = current_uid;
	      		count_buffer[count_i].count = count;

	      		// printf ("buffer uid2: %d count: %d\n", count_buffer[count_i].UID, count_buffer[count_i].count);

				count_i++;
				// printf("switch\n");
				// printf ("uid2: %d count: %d\n", current_uid, count);


				if (block_size/sizeof(Degree) <= count_i){
					// printf("write\n");
					fwrite (count_buffer, sizeof(Degree), count_i, fp_write);
					count_i = 0;
				}

				current_uid = input_buffer[y].UID2;
				count = 1;
	      	}

	    }
	  }
      // printf(" ------------------------------\n");
    }

    count_buffer[count_i].UID = current_uid;
    count_buffer[count_i].count = count;
    // printf("end\n");
    // printf ("buffer uid1: %d count: %d\n", count_buffer[count_i].UID, count_buffer[count_i].count);
    count_i++;
    fwrite (count_buffer, sizeof(Degree), count_i, fp_write);

    return 0;
}

int main(int argc, char **argv){

	// Timing statements
	struct timeb t_begin, t_end;
    long time_spent_ms = 0;

    // Begin timing
    ftime(&t_begin);

    // Program arguments
	char *filename = argv[1];
	int mem_size = atoi(argv[2]);
    int block_size = atoi(argv[3]);
	
	// Sort input file by uid1 and uid2 and store in output1.dat and output.dat respectively
    sort_file (filename, mem_size, block_size, 1);
    sort_file (filename, mem_size, block_size, 2);

    // Get indegree and outdegree and store in indegree.dat and outdegree.dat 
    get_degrees("output1.dat", mem_size, block_size, 1); // outdegree
    get_degrees("output.dat", mem_size, block_size, 2); // indegree

    // End timing statements
    ftime(&t_end);
    time_spent_ms = (long) (1000 *(t_end.time - t_begin.time) + (t_end.millitm - t_begin.millitm))/1000; 
    printf("Total time is:%lu\n", time_spent_ms);  

	return 0;
}