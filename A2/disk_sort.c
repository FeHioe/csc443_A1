#include merge.h
#include stdlib.h

/**
* Compares two records a and b 
* with respect to the value of the integer field f.
* Returns an integer which indicates relative order: 
* positive: record a > record b
* negative: record a < record b
* zero: equal records
*/
int compare (const void *a, const void *b) {
 int a_f = ((const struct Record*)a)->f;
 int b_f = ((const struct Record*)b)->f;
 return (a_f - b_f);
}

/**
* Arguments:
* 1 - an array to sort
* 2 - size of an array
* 3 - size of each array element
* 4 - function to compare two elements of the array
*/
qsort (buffer, total_records, sizeof(Record), compare);

void sort_uid2(char *filename, int total_records) {
	
	Record * buffer = (Record *) calloc(total_records/sizeof(Record), sizeof(Record));

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

	Record * sorted = qsort(buffer, total_records, sizeof(Record), compare);

	for (int i; i < total_records/sizeof(Record); i++){
		printf("Sorted element %d: uid2 %d", i, sorted[i].UID2);
	};

};
int main(int argc, char *argv[]){
	char *filename = argv[1];
	int block_size = atoi(argv[2]);

	sort_uid2(filename, block_size);
}