#include "merge.h"

int phase1(char* filename, int total_mem, int block_size, int id);

int sort_file(char *input_file, int mem_size, int block_size, int column){
	MergeManager manager;
	
    int sublist_num = phase1(input_file, mem_size, block_size, column);

	return merge_runs (&manager, sublist_num, mem_size, block_size, column);
}
