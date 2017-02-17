#include "merge.h"
#include "disk_sort.c"

int main (int argc, char **argv) {
	//process and validate command-line arguments
	
	char *filename = argv[1];
	int total_mem = atoi(argv[2]);
	int block_size = atoi(argv[3]);
	
	FILE *input_file;
	FILE *fp_read;
	MergeManager manager;	
	
	int sublist_num = phase1(filename, total_mem, block_size);

	printf("number of sublist: %d\n", sublist_num);
	
	int blocks_per_mem = total_mem/block_size;	
	Record *output_buffer = (Record *) calloc(block_size/sizeof(Record), sizeof(Record));	
	
	printf("output buf assigned\n");	
	
	blocks_per_mem = blocks_per_mem - 1;
	int blocks_per_input_buf = blocks_per_mem / sublist_num;	

	if (blocks_per_input_buf < 1 ) {
		printf("Error: must have one block per input buffer.");
		exit(1);
	}; 	
	
	
	int i;
	char str[1024];
	
	manager.input_buffers = (Record *) malloc(sublist_num * sizeof(Record *));
	printf("input buf assigned\n");	
		
	manager.current_input_file_positions = (int *) malloc(sublist_num * sizeof(int *));
	printf("current assigned\n");	
	
	for (i=0; i < sublist_num; i++){
		manager.input_buffers[i] = (Record *)malloc(blocks_per_input_buf * block_size);
		printf("input %d assigned\n", i);	
		
		sprintf(str, "sublist%d.dat", i);
		if (!(fp_read = fopen(str, "rb"))) {
    		printf("Error: could not open file for read.\n");
    		exit(1);
  		};
  	
  		manager.current_input_file_positions[i] = fread(manager.input_buffers[i], sizeof(Record), (blocks_per_input_buf * block_size)/sizeof(Record), fp_read);
		printf("read\n");		
	};	
	
	printf("first current_input_pos: %d\n", manager.current_input_file_positions[0]);
	for(i=0; i< (blocks_per_input_buf * block_size)/sizeof(Record)  ;i++){
		printf("printing index %d of first input buffer: %d\n", i, manager.input_buffers[0][i].UID2);
	};
	/*
	
	manager.heap = ;  //keeps 1 from each buffer in top-down order - smallest on top (according to compare function)	
	FILE *inputFP; //stays closed, opens each time we need to reupload some amount of data from disk runs
	int *input_file_numbers;  //we need to know the run id to read from the corresponding run	
	FILE *outputFP; //flushes output from output buffer 
	Record *output_buffer; //buffer to store output elements until they are flushed to disk
	int current_output_buffer_position;  //where to add element in the output buffer
	int output_buffer_capacity; //how many elements max can it hold
	Record **input_buffers; //array of buffers to buffer part of runs
	int input_buffer_capacity; //how many elements max can each input buffer hold
	int *current_input_file_positions; //current position in each sorted run, can use -1 if the run is complete
	int *current_input_buffer_positions; //position in current input buffer
	int *total_input_buffer_elements;  //number of actual elements currently in input buffer - can be less than max capacity
	int current_heap_size;
	int heap_capacity;  //corresponds to the total number of runs (input buffers)
	char output_file_name [MAX_PATH_LENGTH]; //stores name of the file to which to write the final output
	char input_prefix [MAX_PATH_LENGTH] ; //stores the prefix of a path to each run - to concatenate with run id and to read the file
	//initialize all fields according to the input and the results of Phase I
	*/
	return 0;
	//return merge_runs (&manager);
}