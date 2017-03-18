#include "merge.h"



// comparison function that returns true 
// if uid1 is smaller than uid2
int uid1ltuid2(Record * record) {
	return record->UID1 < record->UID2;
}
// comparison function that returns true
// if uid2 is smaller than uid1
int uid2ltuid1(Record * record) {
	return record->UID2 < record->UID1;
}
// function that filters records in the buffer according to the compar
// function
Record * filtertuples(Record * buffer, int total_records int (*compar)(Record * )) {
	Record * lt = (Record *) malloc(total_records*sizeof(Record));
	int i;
	int lti = 0;
	for (i =0; i < total_records; i++){
		if (compar(buffer[i])) {
			lt[lti] = buffer[i];
			lti++;
		}
	}
	return lt;
}

int iterate(char * writeto, char *filename, int total_mem, int block_size, int (*compar)(Record * )) {
  
  FILE *fp_read;
  FILE * fp_write;
  int filesize;
  int result;  
  Record * writebuffer;
  
  if (block_size > total_mem){
	 printf("Error: Block size must be smaller than total memory.\n");
    exit(1);
  };  
  
  if (!(fp_read = fopen(filename, "rb"))) {
    printf("Error: could not open file for read.\n");
    exit(1);
  }

  if (!(fp_write = fopen(writeto, "wb"))){
      printf("Error: could not open file for write.");
      exit(1);
    }
  
  // get file size
  fseek(fp_read, 0L, SEEK_END);
  filesize = ftell(fp_read);
  rewind(fp_read);

  // Check if total memory is sufficient 
  int total_block_num = total_mem/block_size; // M
  int B = filesize/block_size;

  // Partition into K chunks of maximum possible size
  int k = ceil((float)filesize / total_mem);
  printf("filesize: %d total_mem: %d k: %d\n", filesize, total_mem, k);

  // Determine chunk size
  int chunk_size = ceil((float)filesize / k);
 
 //printf("chunk size: %d\n", chunk_size);
  int i;
  for (i=0 ; i < k; i ++){
    // Align chunk with block size 
    Record *buffer = (Record*) calloc (chunk_size, sizeof(Record));
    Record *block_buffer = (Record*) calloc (block_size, sizeof(Record));

    int num_block = 0;
    int block_elements = block_size / sizeof(Record);
    int test = ceil(((float)chunk_size/block_size));
    int buffer_i = 0;

    //printf("record size: %d\n", sizeof(Record));
    //printf("chunk: %d block: %d test:%d block e: %d\n", chunk_size, block_size, test, block_elements);

    while (num_block < test){
      //printf("num_block: %d\n", num_block);      
      
      if ((result = fread(block_buffer, sizeof(Record), block_elements, fp_read)) < 0){
      	printf("Read Error\n");
      };

      
      int y;
      for (y=0; y < 9; y++){
        //printf ("block_buffer element: %d\n", block_buffer[y].UID2);
      };
      
      //printf("block e: %d\n", block_elements);

      int j;
      for (j=0; j < block_elements; j++) {

        buffer[buffer_i].UID1 = block_buffer[j].UID1;
        buffer[buffer_i].UID2 = block_buffer[j].UID2;
        buffer_i++;
      };

      
      for (y=0; y < 9; y++){
        //printf ("buffer element: %d\n", buffer[y].UID2);
      };
      

      if ((num_block+1 == test) && (chunk_size % block_size != 0)){
        block_elements = (chunk_size % block_size) / sizeof(Record);

        if ((result = fread(block_buffer, sizeof(Record), block_elements, fp_read)) < 0){
      	printf("Read Error\n");
        };


        
        int y;
        for (y=0; y < 9; y++){
          //printf ("block_buffer element: %d\n", block_buffer[y].UID2);
        };

        //printf("block e: %d\n", block_elements);
        

        int j;
        for (j=0; j < block_elements; j++) {

          buffer[buffer_i].UID1 = block_buffer[j].UID1;
          buffer[buffer_i].UID2 = block_buffer[j].UID2;
          buffer_i++;
        };

        for (y=0; y < 9; y++){
          //printf ("buffer element: %d\n", buffer[y].UID2);
        };   

      };
      num_block++;

    };

    writebuffer = filtertuples(buffer, ceil((float)chunk_size/sizeof(Record)), compar);

    fwrite(writebuffer, sizeof(Record), ceil((float)chunk_size/sizeof(Record)), fp_write);
    
    free(block_buffer);
    free(buffer);
    free(writebuffer);
  };
  fclose(fp_write);
  fclose(fp_read);
  
  return k;
  
}

int main (int argc, char **argv) {

	if (argc != 4) {
		printf("Program use: filename, total memory, blocksize\n");
		return -1;   
    }

    char *filename = argv[1];
	int total_mem = atoi(argv[2]);
	int block_size = atoi(argv[3]);

	FILE *fp_read;
  	FILE * fp_write;

	if (!(fp_read = fopen(filename, "rb"))) {
    printf("Error: could not open file for read.\n");
    exit(1);
  	}

  	if (!(fp_write = fopen("comparison", "wb"))){
      printf("Error: could not open file for write.");
      exit(1);
    }
    // first get t1.uid1 < t1.uid2 

    // TODO: POSSIBLE TO DO THIS ONCE IN ITERATE (can be updated to improve performance)
    iterate("uid1ltuid2.dat", filename, total_mem, block_size, uid1ltuid2);
    iterate("uid2ltuid1.dat", filename, total_mem, block_size, uid2ltuid1);

    

}