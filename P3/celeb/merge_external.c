#include "merge.h"

//manager fields should be already initialized in the caller
int merge_runs (MergeManager * merger, int total_mem, int block_size, int sublist_num, int id){	
	int  result; //stores SUCCESS/FAILURE returned at the end	
	
	//1. go in the loop through all input files and fill-in initial buffers
	if (init_merge (merger, total_mem, block_size, sublist_num, id)!=SUCCESS)
		return FAILURE;

	while (merger->current_heap_size > 0) { //heap is not empty
		//printf("heap: %d\n", merger->current_heap_size);
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

        //staying on the last slot of the output buffer - next will cause overflow
		if(merger->current_output_buffer_position == merger-> output_buffer_capacity ) {
			if(flush_output_buffer(merger)!=SUCCESS) {
				return FAILURE;			
				merger->current_output_buffer_position=0;
			}	
		}
	
	}

	
	//flush what remains in output buffer
	if(merger->current_output_buffer_position > 0) {
		if(flush_output_buffer(merger)!=SUCCESS)
			return FAILURE;
	}
	
	clean_up(merger);
	return SUCCESS;	
}

int get_top_heap_element (MergeManager * merger, HeapElement * result){
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
				(compare_heap_elements(&(merger->heap[child]),&(merger->heap[child + 1]))>0)) 
			++child;
		
		// compare item with the larger 
		if (compare_heap_elements(&item, &(merger->heap[child]))>0) {
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
		if (compare_heap_elements(&(merger->heap[parent]),&new_heap_element)>0) {
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

int init_merge (MergeManager * manager, int total_mem, int block_size, int sublist_num, int id ) {
	int i;
	int blocks_per_buf = total_mem/(sublist_num+1) / block_size;
	if (blocks_per_buf < 1){
		printf("Memory too small.\n");
		exit(1);
	}

	int buffer_capacity = blocks_per_buf * block_size;

	// Initalize struct values
	manager->heap = (HeapElement *) calloc (sublist_num, sizeof(HeapElement));

	manager->input_file_numbers = (int *) calloc (sublist_num, sizeof(int));
	for (i=0; i < sublist_num; i++){
		manager->input_file_numbers[i] = i;
	};

	manager->output_buffer = (Record *) calloc (buffer_capacity, sizeof(Record));
	manager->current_output_buffer_position = 0;
	manager->output_buffer_capacity = buffer_capacity;
	
	manager->input_buffers = (Record **) calloc(sublist_num, sizeof(Record *));
	for (i = 0; i < sublist_num; i++) { 
		manager->input_buffers[i] = (Record *) calloc (buffer_capacity, sizeof (Record));
	}

	manager->input_buffer_capacity = buffer_capacity;

	manager->current_input_file_positions = (int *) calloc (sublist_num, sizeof(int));
	for (i = 0; i < sublist_num; i++){
		manager->current_input_file_positions[i] = 0;
	}

	manager->current_input_buffer_positions = (int *) calloc (sublist_num, sizeof(int));
	for (i = 0; i < sublist_num; i++){
		manager->current_input_buffer_positions[i] = 0;
	}

	manager->total_input_buffer_elements = calloc (sublist_num, sizeof(int));
	for (i = 0; i < sublist_num; i++){
		manager->total_input_buffer_elements[i] = 0;
	}

	manager->current_heap_size = 0;
	manager->heap_capacity = sublist_num;

	if (id == 2) {
		strcpy(manager->output_file_name, "output.dat");
	} else {
		strcpy(manager->output_file_name, "output1.dat");
	}
	strcpy(manager->input_prefix, "sublist");


	for (i = 0; i < sublist_num; i++){
		refill_buffer(manager, i);

		Record next_record;
		int result = get_next_input_element(manager, i, &next_record);

		if (result == SUCCESS) { 
			if (insert_into_heap(manager, i, &next_record) != SUCCESS)
				return FAILURE;
		} 

		if (result == FAILURE) { 
			return result;
		} 
	}

	return SUCCESS;
}

int flush_output_buffer (MergeManager * manager) {
	manager->outputFP = fopen(manager->output_file_name , "ab");
	if (!manager->outputFP){
		return FAILURE;
	}
/*
	print_buffers(manager);
	int i;
	int count = 0;
	for (i=0; i <manager->current_output_buffer_position; i++){
		printf("position: %d buffer_capacity: %d\n",  manager->current_output_buffer_position, manager-> output_buffer_capacity);

		printf("UID1: %d UID2 %d\n", manager->output_buffer[i]. UID1, manager->output_buffer[i]. UID2);
		count ++;
	}
	printf("size %d count: %d\n", manager->current_output_buffer_position, count);
*/
	if (fwrite(manager->output_buffer, sizeof(Record), manager->current_output_buffer_position, manager->outputFP) == 0) {
		printf("ERROR: output buffer could not be written to disk");
		return FAILURE;
	}
	fflush(manager->outputFP);
	fclose(manager->outputFP);

	manager->current_output_buffer_position = 0;

	return SUCCESS;
}

int get_next_input_element(MergeManager * manager, int file_number, Record *result) {
	if (manager->current_input_buffer_positions[file_number] == manager->total_input_buffer_elements[file_number]){
		int refill = refill_buffer(manager, file_number);
		if (refill != SUCCESS) {
			return refill;
		}
	}

	*result = manager->input_buffers[file_number][manager->current_input_buffer_positions[file_number]];
	manager->current_input_buffer_positions[file_number]++;

	return SUCCESS;
}

int refill_buffer (MergeManager * manager, int file_number) {
	char input_file[MAX_PATH_LENGTH];
	char input_num[MAX_PATH_LENGTH];
	
	strcpy(input_file, "sublist");
	sprintf(input_num, "%d", file_number);
	strcat(input_file, input_num);
	strcat(input_file, ".dat");
	//puts(input_file);

	if (manager->current_input_file_positions[file_number] == -1){
		return EMPTY;
	}

	manager->inputFP = fopen(input_file, "rb");
	if (!manager->inputFP){
		return FAILURE;
	}

	fseek(manager->inputFP, manager->current_input_file_positions[file_number], SEEK_SET);
	int records_read = fread(manager->input_buffers[file_number], sizeof(Record), manager->input_buffer_capacity, manager->inputFP);

	if (records_read != manager->input_buffer_capacity && !feof(manager->inputFP)){
			fprintf(stderr, "refill_buffer - Reading failed. \n");
			return FAILURE;
	}

	manager->current_input_file_positions[file_number] = ftell(manager->inputFP);
	if (feof(manager->inputFP)){
		manager->current_input_file_positions[file_number] = -1;
	}
	fclose(manager->inputFP);

	manager->current_input_buffer_positions[file_number] = 0;
	manager->total_input_buffer_elements[file_number] = records_read;

	return SUCCESS;
}

void clean_up (MergeManager * merger) {
	free(merger->heap);
	free(merger->input_file_numbers);
	free(merger->output_buffer);

	int i;
	for (i = 0; i < merger->heap_capacity; i++){
		free(merger->input_buffers[i]);	
	}

	free(merger->input_buffers);
	free(merger->current_input_file_positions);
}

int compare_heap_elements (HeapElement *a, HeapElement *b) {
	return (a->UID2 - b->UID2);
}
