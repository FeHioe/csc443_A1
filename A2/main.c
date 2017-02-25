#include "merge.h"
#include "disk_sort.c"

int main (int argc, char **argv) {

	//process and validate command-line arguments
	char *filename = argv[1];
	int total_mem = atoi(argv[2]);
	int block_size = atoi(argv[3]);
	
	MergeManager * manager = (MergeManager *) malloc(sizeof(MergeManager));
	
	int sublist_num = phase1(filename, total_mem, block_size);

	printf("number of sublist: %d\n", sublist_num);

	manager->num_input_buffers = sublist_num;
	manager->block_size = block_size;
	manager->total_mem = total_mem;

 	int result = merge_runs(manager);
 	printf("%d");
	return 0;
}