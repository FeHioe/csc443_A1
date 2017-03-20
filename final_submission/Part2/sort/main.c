#include "merge.h"

int phase1(char* filename, int total_mem, int block_size, int id);

int main (int argc, char **argv){
	char *filename = argv[1];
	int total_mem = atoi(argv[2]);
	int block_size = atoi(argv[3]);

	MergeManager manager;
	
    int sublist_num = phase1(filename, total_mem, block_size, 2);

	return merge_runs (&manager, sublist_num, total_mem, block_size, 2);
}
