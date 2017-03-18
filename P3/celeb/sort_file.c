#include "merge.h"
int phase1(char *filename, int total_mem, int block_size, int id);

int sort_file (char *filename, int total_mem, int block_size, int id ) {
	
	MergeManager manager;	

	int sublist_num = phase1(filename, total_mem, block_size, id);

	printf("number of sublist: %d\n", sublist_num);

	return merge_runs (&manager, total_mem, block_size, sublist_num, id);
}
