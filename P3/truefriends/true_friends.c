#include "merge.h"
#include "sort_file.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// comparison function that returns true 
// if uid1 is smaller than uid2
int uid1ltuid2(Record * record) {
	return (record->UID1 < record->UID2);
}

// comparison function that returns true
// if uid2 is smaller than uid1
int uid2ltuid1(Record * record) {
	return (record->UID2 < record->UID1);
}

// comparison function that returns true if 
// the given two records are true friends
int true_friend(Record * record, Record * record2) {
	return ((record->UID1 == record2->UID2) && (record2->UID1 == record->UID2));
}
/*
// function that filters records in the buffer according to the compar
// function
Record * filtertuples(Record * buffer, int total_records, int (*compar)(Record * )) {
	Record * lt = (Record *) malloc(total_records*sizeof(Record));
	int i;
	int lti = 0;
	for (i =0; i < total_records; i++){
		if (compar(&buffer[i])) {
			lt[lti] = buffer[i];
			printf("added tuple: uid1: %d, uid2: %d\n", lt[lti].UID1, lt[lti].UID2);
			lti++;
		}
	}
	return lt;
}
*/
int iterate(char * writeto, char *filename, int total_mem, int block_size, int (*compar)(Record * )) {
  
  FILE *fp_read;
  FILE * fp_write;
  int filesize;
  int result;  
  //Record * writebuffer;
  int lti;
  int total_records;
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
  //int total_block_num = total_mem/block_size; // M
  //int B = filesize/block_size;

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
    
    //writebuffer = filtertuples(buffer, ceil((float)chunk_size/sizeof(Record)), compar);
    total_records = ceil((float)chunk_size/sizeof(Record));
	 Record * lt = (Record *) malloc(total_records*sizeof(Record));
	 lti = 0;
	 for (int i =0; i < total_records; i++){
	 	if (compar(&buffer[i])) {
			lt[lti] = buffer[i];
			if (lt[lti].UID1 == 0 || lt[lti].UID2 == 0) { printf("added tuple: uid1: %d, uid2: %d\n", lt[lti].UID1, lt[lti].UID2);}
			lti++;
		}
	}
    fwrite(lt, sizeof(Record), lti, fp_write);
    
    free(block_buffer);
    free(buffer);
    free(lt);
  };
  fclose(fp_write);
  fclose(fp_read);
  
  return k;
  
}
// Join: t1.uid1 = t2.uid2 and t2.uid1 = t1.uid2
// filename represents R1
// fileame2 represents R2
// writeto filename to write query result to
int true_friend_query(char * table1, char * table2, char * writeto, int total_mem, int block_size) {
  // see which table is smaller
  int count = 0;
  FILE *fp_readtable1;
  FILE *fp_readtable2;
  FILE * fp_R;
  FILE * fp_S;
  FILE * fp_write;
  int table1_filesize;
  int table2_filesize;
  int R_filesize;
  int S_filesize;
  int result;  
  // Record * writebuffer;
  
  if (block_size > total_mem){
   printf("Error: Block size must be smaller than total memory.\n");
    exit(1);
  };  
  
  if (!(fp_readtable1 = fopen(table1, "rb"))) {
    printf("Error: could not open file for read.\n");
    exit(1);
  }
  if (!(fp_readtable2 = fopen(table2, "rb"))) {
    printf("Error: could not open file for read.\n");
    exit(1);
  }

  if (!(fp_write = fopen(writeto, "wb"))){
      printf("Error: could not open file for write.");
      exit(1);
    }
  
  // get file size
  fseek(fp_readtable1, 0L, SEEK_END);
  table1_filesize = ftell(fp_readtable1);
  rewind(fp_readtable1);

  fseek(fp_readtable2, 0L, SEEK_END);
  table2_filesize = ftell(fp_readtable2);
  rewind(fp_readtable2);

  if (table1_filesize < table2_filesize) {
    fp_S = fp_readtable1;
    S_filesize = table1_filesize;
    fp_R = fp_readtable2;
    R_filesize = table2_filesize;
  } else {
    if (table1_filesize == table2_filesize) {
      printf("tables are both of equal size\n");
    }
    fp_S = fp_readtable2;
    S_filesize = table2_filesize;
    fp_R = fp_readtable1;
    R_filesize = table1_filesize;
  }

  // Check if total memory is sufficient 
  int total_block_num = total_mem/block_size; // M
  //int B = filesize/block_size;

  // divide table R into chunks of size M - 1
  int c_r = ceil((float) R_filesize / (total_block_num - 1));
  //printf("filesize: %d total_mem: %d k: %d\n", filesize, total_mem, c_r);

  // Determine chunk size
  int chunk_size = ceil((float) R_filesize / c_r);
 
 //printf("chunk size: %d\n", chunk_size);
  int i;
  // for each chunk c_r of R of size M - 1
  for (i=0 ; i < c_r; i ++){
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
      
      if ((result = fread(block_buffer, sizeof(Record), block_elements, fp_R)) < 0){
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

        if ((result = fread(block_buffer, sizeof(Record), block_elements, fp_R)) < 0){
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

    int num_pages = S_filesize/block_size;
    // for each page of S
    for (int i = 0; i < num_pages; i++) {
      Record * S_buffer = (Record * ) malloc(block_size);
      if ((result = fread(S_buffer, sizeof(Record), block_elements, fp_S)) < 0){
        printf("Read Error\n");
      };
      // for each tuple in S
      for (int i = 0; i < block_elements; i++) {
          // for each tuple in R
          for (int j = 0; j < buffer_i; j++){
          	    if (true_friend(&S_buffer[i], &buffer[j]))  {
                  printf("True friends: %d %d", S_buffer[i].UID1, buffer[j].UID2);
                  // DO SELECT HERE
                  fwrite(&S_buffer[i], sizeof(Record), 1, fp_write);
                  count++;
                  break;
              }
          }
      }

      free(S_buffer);
    }

    free(block_buffer);
    free(buffer);
    //free(writebuffer);
  }
  fclose(fp_write);
  fclose(fp_S);
  fclose(fp_R);
  printf("Count: %d", count);
  return c_r;

}
int main(int argc, char **argv) {

	if (argc != 4) {
		printf("Program use: filename, total memory, blocksize\n");
		return -1;   
    }

    char *filename = argv[1];
	  int total_mem = atoi(argv[2]);
	  int block_size = atoi(argv[3]);


    // first get t1.uid1 < t1.uid2 

    // TODO: POSSIBLE TO DO THIS ONCE IN ITERATE (can be updated to improve performance)
    // TABLE 1: t1.uid1 < t1.uid2
    if (iterate("table1.dat", filename, total_mem, block_size, uid1ltuid2) <= 0) {
    	printf("Error\n");
    }
    // TABLE 2: t2.uid2 < t2.uid1
    if (iterate("table2.dat", filename, total_mem, block_size, uid2ltuid1) <= 0) {
    	printf("Error\n");
    }

    // to optimize: sort by uid1 in relation 1, then sort by uid2 in relation 2
     //sort_file("table1.dat", "sortedbyuid1table1.dat", total_mem, block_size, 1);
     //sort_file("table2.dat", "sortedbyuid2table2.dat", total_mem, block_size, 2);

    //true_friend_query("sortedbyuid1table1.dat", "sortedbyuid2table2.dat", "result.dat", total_mem, block_size);
	return 1;
}