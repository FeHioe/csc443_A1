#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include "merge.h"

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

int compare_uid1 (const void *a, const void *b) {
 int a_f = ((const Record *)a)->UID1;
 int b_f = ((const Record *)b)->UID1;
 return (a_f - b_f);
}

void sort_array_by_uid2(Record * buffer, int total_records) {
	qsort(buffer, total_records, sizeof(Record), compare);
};

void sort_array_by_uid1(Record * buffer, int total_records) {
  qsort(buffer, total_records, sizeof(Record), compare_uid1);
};

int phase1(char* filename, int total_mem, int block_size, int id){

	FILE *fp_read;
	FILE *fp_write;
	int filesize; 

    if (block_size > total_mem){
  	   printf("Error: Block size must be smaller than total memory.\n");
      exit(1);
    };  

    if (!(fp_read = fopen(filename, "rb"))) {
      printf("Error: could not open file for read.\n");
      exit(1);
    }

    // get file size
    fseek(fp_read, 0, SEEK_END);
    filesize = ftell(fp_read);
    int all_records = filesize / sizeof(Record);
    rewind(fp_read);  

  // Check if total memory is sufficient 
  int total_block_num = total_mem/block_size; // M
  int B = filesize/block_size;
  if (B > (total_block_num * total_block_num)) {
		printf("file cannot be sorted given the available memory\n");
		exit(1);
  }

    int chunk_size =  total_mem / block_size / 2;
    int k_records = chunk_size * block_size / sizeof(Record);
    int remaining_records = all_records;
    int records_read;
    int k = 0;

    Record * buffer = (Record *) calloc (k_records, sizeof (Record));
    while(0 < remaining_records){
        records_read = fread(buffer, sizeof(Record), k_records, fp_read);
  
		if (0 < records_read){
    	    char string[1024];
			char file_number_str[1024];

			if (id == 1) {
    	    	sort_array_by_uid1(buffer, records_read);

				strcpy(string, "sublist1_");
				sprintf(file_number_str,"%d",k);
			
				strcat(string, file_number_str);
				strcat(string, ".dat");
			} else {
				sort_array_by_uid2(buffer, records_read);
				strcpy(string, "sublist2_");
				sprintf(file_number_str,"%d",k);
			
				strcat(string, file_number_str);
				strcat(string, ".dat");
			}
    	    
    	    
    	    if (!(fp_write = fopen (string , "wb"))) {  
    	    	printf ("Could not open sublist file \n");
    	    	exit(1);
    	    }
    	    fwrite (buffer, sizeof(Record), records_read, fp_write);
    	    fclose(fp_write);
			
			remaining_records -= records_read;
			k ++;
		} 
	}

	free(buffer);
	fclose(fp_read);

	return k;
}


