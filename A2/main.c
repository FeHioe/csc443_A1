#include "merge.h"
#include "disk_sort.c"

int main (int argc, char **argv) {
	//process and validate command-line arguments
	
	char *filename = argv[1];
	int total_mem = atoi(argv[2]);
	int block_size = atoi(argv[3]);
	
	FILE *input_file;
	FILE *fp_read;
	MergeManager * manager = (MergeManager *) malloc(sizeof(MergeManager));
	
	int sublist_num = phase1(filename, total_mem, block_size);

	printf("number of sublist: %d\n", sublist_num);
	
	int blocks_per_mem = total_mem/block_size;	
	Record *output_buffer = (Record *) calloc(block_size/sizeof(Record), sizeof(Record));	
	manager->output_buffer_capacity = block_size/sizeof(Record);
	printf("output buf assigned\n");	
	
	blocks_per_mem = blocks_per_mem - 1;
	int blocks_per_input_buf = blocks_per_mem / sublist_num;	

	if (blocks_per_input_buf < 1 ) {
		printf("Error: must have one block per input buffer.");
		exit(1);
	}; 	
	manager->output_file_name = "output.dat";
	if (!(manager->outputFP = fopen(manager->output_file_name, "wb"))){
      printf("Error: could not open file for write.");
      exit(1);
	}
	manager->input_buffer_capacity = (blocks_per_input_buf * block_size)/sizeof(Record);
	int i;
	char str[1024];
	
	manager->input_buffers = (Record *) malloc(sublist_num * sizeof(Record *));
	printf("input buf assigned\n");	
		
	manager->current_input_file_positions = (int *) malloc(sublist_num * sizeof(int *));
	printf("current assigned\n");	
	
	for (i=0; i < sublist_num; i++){
		manager->input_buffers[i] = (Record *)malloc(blocks_per_input_buf * block_size);
		printf("input %d assigned\n", i);	
		
		sprintf(str, "sublist%d.dat", i);
		if (!(fp_read = fopen(str, "rb"))) {
    		printf("Error: could not open file for read.\n");
    		exit(1);
  		};
  	
  		// TODO: manager->current_input_file_positions[i] = 
  		num_elements_read = fread(manager->input_buffers[i], sizeof(Record), (blocks_per_input_buf * block_size)/sizeof(Record), fp_read);
  		manager->total_input_buffer_elements[i] = num_elements_read;
  		manager->current_input_file_positions[i] = ftell(fp_read);
  		manager->current_input_buffer_positions[i] = 0;
  		total_input_buffer_elements
  		fclose(fp_read);
		printf("read\n");		
	};	
	/* Check to see if buffer is filled correctly
	printf("first current_input_pos: %d\n", manager.current_input_file_positions[0]);
	for(i=0; i< (blocks_per_input_buf * block_size)/sizeof(Record)  ;i++){
		printf("printing index %d of first input buffer: %d\n", i, manager.input_buffers[0][i].UID2);
	};
	*/
 	// initializes heap taking 1 top element from each buffer 
 	manager->heap = (HeapElement *) malloc(sizeof(HeapElement) * sublist_num);
 	manager->current_heap_size = -1;
 	manager->heap_capacity = sublist_num;

 	for (i=0; i < sublist_num; i++) {
 		current_input_buffer_pos = manager->current_input_buffer_pos[i];
 		insert_into_heap(manager, i, manager->input_buffers[i][current_input_buffer_pos]);
 		manager->current_input_buffer_pos[i]++; // increment buffer position for this input buffer
 	}
 	// take top element from the heap, and place it in output buffer
 	int result = merge_runs(manager);
 	printf("%d");
	return 0;
	//return merge_runs (&manager);
}