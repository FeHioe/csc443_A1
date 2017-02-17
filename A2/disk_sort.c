#include "merge.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/**
* Compares two records a and b 
* with respect to the value of the integer field f.
* Returns an integer which indicates relative order: 
* positive: record a > record b
* negative: record a < record b
* zero: equal records
*/

int compare (const void *a, const void *b) {
 int a_f = ((const Record *)a)->UID2;
 int b_f = ((const Record *)b)->UID2;
 return (a_f - b_f);
}

void sort_array_by_uid2(Record * buffer, int total_records) {
	qsort(buffer, total_records, sizeof(Record), compare);
   int i;
	for (i =0; i < total_records; i++){
		printf("Sorted element %d: uid2 %d\n", i, buffer[i].UID2);
	};
};

int main(int argc, char *argv[]){
  /* PART 1
	char *filename = argv[1];
	int block_size = atoi(argv[2]);
   FILE *fp_read;
	if (!(fp_read = fopen(filename, "rb"))) {
    	printf("Error: could not open file for read.\n");
    	exit(1);
    }
    
   int total_records = block_size/sizeof(Record);
   Record * buffer = (Record *) calloc(total_records, sizeof(Record));
   int result = fread(buffer, sizeof(Record), total_records, fp_read);
	printf("result: %d\n", result);
	if (result != total_records){
		printf("Error: sizing issue.\n");
		exit(1);
	}
	sort_array_by_uid2(buffer, total_records);

  return 0;
  */
  
  char *filename = argv[1];
  int total_mem = atoi(argv[2]); // add plus 5MB
  int block_size = atoi(argv[3]);
  FILE *fp_read;
  FILE * fp_write;
  int result;
  int filesize;
  
  if (!(fp_read = fopen(filename, "rb"))) {
    printf("Error: could not open file for read.\n");
    exit(1);
  }
  // get file size
  fseek(fp_read, 0L, SEEK_END);
  filesize = ftell(fp_read);
  rewind(fp_read);

  // Check if total memory is sufficient 
  int total_block_num = total_mem/block_size; // M
  int B = filesize/block_size;
  printf("B is %d\n", B);
  printf("M is %d\n", total_block_num);
  if (B > (total_block_num * total_block_num)) {
		printf("file cannot be sorted given the available memory\n");
		exit(1);
  }

  // Partition into K chunks of maximum possible size
  int k = ceil((float)filesize / total_mem);
  printf("filesize: %d total_mem: %d k: %d\n", filesize, total_mem, k);

  // Determine chunk size
  int chunk_size = ceil((float)filesize / k);

  int i;
  char str[1024];
  for (i=0 ; i < k; i ++){
    // Align chunk with block size 

    //printf ("before buffers\n");
    Record *buffer = (Record*) calloc (chunk_size, sizeof(Record));
    Record *block_buffer = (Record*) calloc (block_size, sizeof(Record));
    //printf("after buffers\n");

    int num_block = 0;
    int block_elements = block_size / sizeof(Record);
    int test = ((int)chunk_size/block_size);
    int buffer_i = 0;

    //printf("before read\n");
    printf("record size: %d\n", sizeof(Record));
    printf("chunk: %d block: %d test:%d block e: %d\n", chunk_size, block_size, test, block_elements);

    while ((num_block < test ) && (result = fread(block_buffer, sizeof(Record), block_elements, fp_read) > 0) ){
      num_block++;

      //printf("read\n");
      /*
      if (num_block == (chunk_size/block_size) && (chunk_size % block_size != 0)){
        block_elements = (chunk_size % block_size) / sizeof(Record);
      };
      */
      int y;
      for (y=0; y < 9; y++){
        printf ("block_buffer element: %d\n", block_buffer[y].UID2);
      };

      //printf("add records\n");

      printf("block e: %d\n", block_elements);
      int j;
      for (j=0; j < block_elements; j++) {

        buffer[buffer_i].UID1 = block_buffer[j].UID1;
        buffer[buffer_i].UID2 = block_buffer[j].UID2;
        buffer_i++;
      };
     // printf("added\n");

      for (y=0; y < 9; y++){
        printf ("buffer element: %d\n", buffer[y].UID2);
      };

    };

    sort_array_by_uid2(buffer, ceil((float)chunk_size/sizeof(Record)));
    sprintf(str, "sublist%d.dat", i);

    if (!(fp_write = fopen(str, "wb"))){
      printf("Error: could not open file for write.");
      exit(1);
    }

    fwrite(buffer, sizeof(Record), ceil((float)chunk_size/sizeof(Record)), fp_write);
    fclose(fp_write);
    free(block_buffer);
    free(buffer);
  };
  

  /*
  int i = 1;
  char str[1024];
  Record * buffer = (Record *) calloc(total_mem/sizeof(Record), sizeof(Record));

  while ((result = fread(buffer, sizeof(Record), total_mem/sizeof(Record), fp_read)) > 0){ 

  	sort_array_by_uid2(buffer, result);
   	sprintf(str, "sublist%d.dat", i);

    if (!(fp_write = fopen(str, "wb"))){
      printf("Error: could not open file for write.");
      exit(1);
    }

    fwrite(buffer, sizeof(Record), result, fp_write);
    fclose(fp_write);
    i++; 

  }
  */

  fclose(fp_read);
  
  return 0;
  
}