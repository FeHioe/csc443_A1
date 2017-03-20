#include "merge.h"

//manager fields should be already initialized in the caller
int merge_runs (MergeManager * merger){	
	int  result; //stores SUCCESS/FAILURE returned at the end	
	//printf("*************IN MERGE RUNS *************\n"); 
	//1. go in the loop through all input files and fill-in initial buffers
	if (init_merge (merger)!=SUCCESS)
		return FAILURE;

	while (merger->current_heap_size > 0) { //heap is not empty
		HeapElement smallest;
		Record next; //here next comes from input buffer
		
		if(get_top_heap_element (merger, &smallest)!=SUCCESS)
			return FAILURE;

		result = get_next_input_element (merger, smallest.run_id, &next);
		
		if (result==FAILURE)
			return FAILURE;

		if(result==SUCCESS) {//next element exists, may also return EMPTY
			if(insert_into_heap (merger, smallest.run_id, &next)!=SUCCESS)
				return FAILURE;
		}		

			merger->output_buffer [merger->current_output_buffer_position].UID1=smallest.UID1;
			merger->output_buffer [merger->current_output_buffer_position].UID2=smallest.UID2;
			merger->current_output_buffer_position++;	
	
		//printf("current output buffer position: %d, buffer capacity: %d\n", merger->current_output_buffer_position, merger->output_buffer_capacity);
        //staying on the last slot of the output buffer - next will cause overflow
		if(merger->current_output_buffer_position == merger-> output_buffer_capacity ) {
			//printf("BUFFER IS FULL\n");
			if(flush_output_buffer(merger)!=SUCCESS) {
				return FAILURE;			
			}	
		}
	
	}
	
	//flush what remains in output buffer
	if(merger->current_output_buffer_position > 0) {
		printf("finished, empty buffer\n");
		if(flush_output_buffer(merger)!=SUCCESS)
			return FAILURE;
	}
	
	clean_up(merger);
	return SUCCESS;	
}


int get_top_heap_element (MergeManager * merger, HeapElement * result){
	//printf("*************GET TOP HEAP ELEMENT *************\n");
	HeapElement item;
	int child, parent;

	if(merger->current_heap_size == 0){
		printf( "UNEXPECTED ERROR: popping top element from an empty heap\n");
		return FAILURE;
	}

	*result=merger->heap[0];  //to be returned

	//now we need to reorganize heap - keep the smallest on top
	item = merger->heap [--merger->current_heap_size]; // to be reinserted 

	parent =0;
	while ((child = (2 * parent) + 1) < merger->current_heap_size) {
		// if there are two children, compare them 
		if (child + 1 < merger->current_heap_size && 
				(compare_heap_elements(&(merger->heap[child]),&(merger->heap[child + 1]), merger)>0)) 
			++child;
		
		// compare item with the larger 
		if (compare_heap_elements(&item, &(merger->heap[child]), merger)>0) {
			merger->heap[parent] = merger->heap[child];
			parent = child;
		} 
		else 
			break;
	}
	merger->heap[parent] = item;
	
	return SUCCESS;
}

int insert_into_heap (MergeManager * merger, int run_id, Record *input){
	//printf("*************INSERT INTO HEAP *************\n");
	HeapElement new_heap_element;
	int child, parent;
	new_heap_element.UID1 = input->UID1;
	new_heap_element.UID2 = input->UID2;
	new_heap_element.run_id = run_id;
	
	if (merger->current_heap_size == merger->heap_capacity) {
		printf( "Unexpected ERROR: heap is full\n");
		return FAILURE;
	}
  	
	child = merger->current_heap_size++; /* the next available slot in the heap */
	
	while (child > 0) {
		parent = (child - 1) / 2;
		if (compare_heap_elements(&(merger->heap[parent]),&new_heap_element, merger)>0) {
			merger->heap[child] = merger->heap[parent];
			child = parent;
		} 
		else 
			break;
	}
	merger->heap[child]= new_heap_element;	
	return SUCCESS;
}


/*
** TO IMPLEMENT
*/

int init_merge (MergeManager * manager) { 
	//printf("*************IN INIT *************\n");
	int sublist_num = manager->num_input_buffers;
	int block_size = manager->block_size;
	int total_mem = manager->total_mem;
	int num_elements_read;
	int current_input_buffer_pos;
	FILE *fp_read;

	int blocks_per_mem = total_mem/block_size;	
	Record *output_buffer = (Record *) calloc(block_size/sizeof(Record), sizeof(Record));	
	manager->output_buffer = output_buffer;
	manager->output_buffer_capacity = block_size/sizeof(Record);
	
	manager->total_input_buffer_elements = (int *) malloc(sizeof(int) * sublist_num);
	manager->current_input_file_positions = (int *) malloc(sizeof(int) * sublist_num);
	manager->current_input_buffer_positions =  (int *)  malloc(sizeof(int) * sublist_num);
	printf("output buf assigned\n");	
	
	blocks_per_mem = blocks_per_mem - 1;
	int blocks_per_input_buf = blocks_per_mem / sublist_num;	

	if (blocks_per_input_buf < 1 ) {
		printf("Error: must have one block per input buffer.");
		return FAILURE;
	}
	//strcpy(manager->output_file_name, "output.dat");
	if (!(manager->outputFP = fopen(manager->output_file_name, "wb"))){
      printf("Error: could not open file for write. Line 151 merge external");
      return FAILURE;
	}
	manager->input_buffer_capacity = (blocks_per_input_buf * block_size)/sizeof(Record);
	int i;
	char str[1024];
	
	manager->input_buffers = (Record **) malloc(sublist_num * sizeof(Record *));
	printf("input buf assigned\n");	
		
	manager->current_input_file_positions = (int *) malloc(sublist_num * sizeof(int *));
	printf("current assigned\n");	
	
	for (i=0; i < sublist_num; i++){
		manager->input_buffers[i] = (Record *)malloc(blocks_per_input_buf * block_size);
		printf("input %d assigned\n", i);	
		
		sprintf(str, "sublist%d.dat", i);
		if (!(fp_read = fopen(str, "rb"))) {
    		printf("Error: could not open file for read. Line 170 merge external\n");
    		return FAILURE;
  		};
  		printf("fopen done\n");
  		// TODO: manager->current_input_file_positions[i] = 
  		num_elements_read = fread(manager->input_buffers[i], sizeof(Record), (blocks_per_input_buf * block_size)/sizeof(Record), fp_read);
  		manager->total_input_buffer_elements[i] = num_elements_read;
  		manager->current_input_file_positions[i] = ftell(fp_read);
  		manager->current_input_buffer_positions[i] = 0;
  		fclose(fp_read);
		printf("read\n");		
	};	
   printf("done setting input buffers\n");
 	// initializes heap taking 1 top element from each buffer 
 	manager->heap = (HeapElement *) malloc(sizeof(HeapElement) * sublist_num);
 	manager->heap_capacity = sublist_num;
   printf("done heap stuff\n");
 	for (i=0; i < sublist_num; i++) {
 		current_input_buffer_pos = manager->current_input_buffer_positions[i];
 		insert_into_heap(manager, i, (&manager->input_buffers[i][current_input_buffer_pos]));
 		manager->current_input_buffer_positions[i]++; // increment buffer position for this input buffer
 	}
	manager->current_heap_size = sublist_num;
	return SUCCESS;
}

int flush_output_buffer (MergeManager * manager) {
	//int num_records_to_write;
	
	//printf("*************IN FLUSH OUTPUT BUFFER************* current out buffer position: %d\n", manager->current_output_buffer_position);
		//for (int i = 0; i < manager->current_output_buffer_position; i++) {
			//	printf("uid1: %d, added uid2: %d\n", (manager->output_buffer[i]).UID1, (manager->output_buffer[i]).UID2);		
		//}

	if (fwrite(manager->output_buffer, sizeof(Record), manager->current_output_buffer_position, manager->outputFP) != (manager->current_output_buffer_position)) {
		printf("ERROR: output buffer could not be written to disk\n");
		return FAILURE;
	}
	fflush(manager->outputFP);
	manager->current_output_buffer_position = 0;
	return SUCCESS;
}

int get_next_input_element(MergeManager * manager, int file_number, Record *result) {
	//printf("**************  GET NEXT INPUT ELEMENT *************\n");
	int num_elements_read;
	char str[1024];
	int current_buffer_pos;
	FILE *fp_read;
	// See if the corresponding input buffer has anything left
	if (manager->total_input_buffer_elements[file_number] < 0) {
		printf( "Unexpected ERROR: negative number of elements in input buffer\n");
		return FAILURE;
	}

	if (manager->total_input_buffer_elements[file_number] == 0) {
		sprintf(str, "sublist%d.dat", file_number);
		if (!(fp_read = fopen(str, "rb"))) {
    		printf("Error: could not open file for read.\n");
    		exit(1);
  		};
  		fseek(fp_read, manager->current_input_file_positions[file_number], SEEK_SET);
  		num_elements_read = fread(manager->input_buffers[file_number], sizeof(Record), manager->input_buffer_capacity, fp_read);

  		if (num_elements_read < 1) {
  			fclose(fp_read);
  			manager->total_input_buffer_elements[file_number] = 0;
  			manager->current_input_file_positions[file_number] = -1;
  			manager->current_input_buffer_positions[file_number] = -1;
  			manager->heap_capacity--;
  			printf("file exhausted, one run done");
  			return EMPTY;
  		}
  		manager->total_input_buffer_elements[file_number] = num_elements_read;
  		manager->current_input_file_positions[file_number] += ftell(fp_read);
  		manager->current_input_buffer_positions[file_number] = 0;
  		fclose(fp_read);
  	}

	current_buffer_pos = manager->current_input_buffer_positions[file_number];
	*result = manager->input_buffers[file_number][current_buffer_pos];
	manager->current_input_buffer_positions[file_number]++;
	manager->total_input_buffer_elements[file_number]--;
	
	return SUCCESS;
}


int refill_buffer (MergeManager * manager, int file_number) {
	return SUCCESS;
}

void clean_up (MergeManager * manager) {
	//printf("*************IN FREE *************\n");
	fclose(manager->outputFP);
	free(manager->heap);
	free(manager->output_buffer);
	for (int i = 0; i < (manager->num_input_buffers); i++) {
		free(manager->input_buffers[i]);
	}
	free(manager->current_input_file_positions);
	free(manager->current_input_buffer_positions);
	free(manager->total_input_buffer_elements);
	free(manager);
}
// if a bigger than b, return greater than 0
int compare_heap_elements (HeapElement *a, HeapElement *b, MergeManager * merger) {
	if (merger->sortby == 1) {
		return (a->UID1 - b->UID1);
	} else if (merger->sortby == 2) {
		return (a->UID2 - b->UID2);
	} else {
		printf("Error: Cannot sort by given uid");
		exit(1);
	}
}