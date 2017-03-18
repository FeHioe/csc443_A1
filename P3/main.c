#include "merge.h"



// return only the tuples where uid1 < uid2
int lessthan(Record * record)
void filtertuples(FILE * readfrom, FILE * writeto, int (*compar)(Record * ))

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

    filtertuples(fp_read, fp_write, lessthan);

}