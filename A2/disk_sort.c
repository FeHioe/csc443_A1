#include "merge.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  
  int num_records_in_blocksize = block_size/sizeof(Record);
  
  // ?
  int max_num_records = (total_mem * total_mem) / (sizeof(Record) * block_size); 
  
  if (max_num_records < total_block_num){ //double check this
    printf("Error not enough memory\n");
    exit(1);
  };
  
  // Partition into K chunks of maximum possible size
  int i = 1;
  char str[1024];
   // TODO: data should be read and written in terms of blocks
   // need to align blocks and records
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
  
  fclose(fp_read);
  free(buffer);
  
  return 0;
  
}