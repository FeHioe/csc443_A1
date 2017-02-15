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

/**
* Arguments:
* 1 - an array to sort
* 2 - size of an array
* 3 - size of each array element
* 4 - function to compare two elements of the array
*/


void sort_array_by_uid2(Record * buffer, int total_records) {
	
	// Record * buffer = (Record *) calloc(total_records/sizeof(Record), sizeof(Record));
   /*
	FILE *fp_read;
	if (!(fp_read = fopen(filename, "rb"))) {
    	printf("Error: could not open file for read.\n");
    	exit(1);
    }
    
    int result = fread(buffer, sizeof(Record), total_records, fp_read);
	printf("result: %d\n", result);
	if (result != total_records){
		printf("Error: sizing issue.\n");
		exit(1);
	}
	*/
	qsort(buffer, total_records, sizeof(Record), compare);
   int i;
	for (i =0; i < total_records; i++){
		printf("Sorted element %d: uid2 %d", i, buffer[i].UID2);
	};

};
int main(int argc, char *argv[]){
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
}