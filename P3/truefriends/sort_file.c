#include "disk_sort.h"
#include "merge.h"


// filename is filename to read from 
// sortby is the uid to sort by either 1 or 0
// writeto is the filename to write the sorted file to 
int sort_file(char * filename, char * writeto, int total_mem, int block_size, int sortby) {
	
	MergeManager * manager = (MergeManager *) malloc(sizeof(MergeManager));
	
	int sublist_num = phase1(filename, total_mem, block_size);

	printf("number of sublist: %d\n", sublist_num);
	strcpy(manager->output_file_name, writeto);
	manager->sortby = sortby;
	manager->num_input_buffers = sublist_num;
	manager->block_size = block_size;
	manager->total_mem = total_mem;

 	int result = merge_runs(manager);
 	printf("finished returned %d\n", result);
	return 0;
}