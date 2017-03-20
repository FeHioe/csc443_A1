#include "merge.h"

//manager fields should be already initialized in the caller
int merge_runs (MergeManager * merger, int sublist_num, int mem_size, int block_size, int column){	
	int  result; //stores SUCCESS/FAILURE returned at the end	
	
	//1. go in the loop through all input files and fill-in initial buffers
	if (init_merge (merger, sublist_num, mem_size, block_size, column)!=SUCCESS){
		return FAILURE;
	}
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
				(compare_heap_elements(&(merger->heap[child]),&(merger->heap[child + 1]), merger->column)>0)) 
			++child;
		
		// compare item with the larger 
		if (compare_heap_elements(&item, &(merger->heap[child]), merger->column)>0) {
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
		if (compare_heap_elements(&(merger->heap[parent]),&new_heap_element, merger->column)>0) {
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

int init_merge (MergeManager * manager, int sublist_num, int mem_size, int block_size, int column) {
	
	int blocks_per_buffer = mem_size/(sublist_num+1) / block_size;
	int buffer_capacity = blocks_per_buffer * block_size / sizeof(Record);

	manager->heap = (HeapElement *) calloc (sublist_num, sizeof (HeapElement));

	manager->input_file_numbers = (int *) calloc (sublist_num, sizeof (int));
	for (int i = 0; i < sublist_num; i++){
		manager->input_file_numbers[i] = i;
	}

	manager->output_buffer = (Record *) calloc (buffer_capacity, sizeof (Record));
	manager->current_output_buffer_position = 0;
	manager->output_buffer_capacity = buffer_capacity;

	manager->input_buffers = (Record **) calloc (sublist_num, sizeof (Record *));
	for (int i = 0; i < sublist_num; i++) { 
		manager->input_buffers[i] = (Record *) calloc (buffer_capacity, sizeof (Record));
	}
	manager->input_buffer_capacity = buffer_capacity;

	manager->current_input_file_positions = (int *) calloc (sublist_num, sizeof (int));
	for (int i = 0; i < sublist_num; i++){
		manager->current_input_file_positions[i] = 0;
	}

	manager->current_input_buffer_positions = (int *) calloc (sublist_num, sizeof (int));
	for (int i = 0; i < sublist_num; i++){
		manager->current_input_buffer_positions[i] = 0;
	}

	manager->total_input_buffer_elements = calloc (sublist_num, sizeof (int));
	for (int i = 0; i < sublist_num; i++){
		manager->total_input_buffer_elements[i] = 0;
	}

	manager->current_heap_size = 0;
	manager->heap_capacity = sublist_num;

	if (column == 1){
		strcpy(manager->input_prefix, "sublist1_");
		strcpy(manager->output_file_name, "output1.dat");
	}
	else{
		strcpy(manager->input_prefix, "sublist2_");
		strcpy(manager->output_file_name, "output2.dat");
	}
	manager->column = column;
	
	manager->file_capacity = (int *) calloc (sublist_num, sizeof (int));
	for (int i = 0; i < sublist_num; i++){
		char filename[MAX_PATH_LENGTH];
		char file_number_str[MAX_PATH_LENGTH];
	
		strcpy(filename, manager->input_prefix);
		sprintf(file_number_str,"%d",i);
		strcat(filename, file_number_str);
		strcat(filename, ".dat");

		FILE *fp_read = fopen(filename, "rb");
		if (!fp_read){
			return FAILURE;
		}
	
		fseek(fp_read, 0, SEEK_END);
		int filesize = ftell(fp_read);
		int all_records = filesize / sizeof(Record);
		fseek(fp_read, 0, SEEK_SET); 
	
		fclose(fp_read);
		manager->file_capacity[i] = all_records;
	}
	
	for (int i = 0; i < sublist_num; i++){
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
	manager->outputFP = fopen (manager->output_file_name , "ab");
	if (!manager->outputFP){
		return FAILURE;
	}

	if (fwrite(manager->output_buffer, sizeof(Record), manager->current_output_buffer_position, manager->outputFP) == 0) {
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
	strcpy(input_file, manager->input_prefix);
	sprintf(input_num,"%d",file_number);
	strcat(input_file, input_num);
	strcat(input_file, ".dat");

	if (manager->current_input_file_positions[file_number] == -1){
		return EMPTY;
	}
	
	manager->inputFP = fopen(input_file, "rb");
	if (!manager->inputFP){
		return FAILURE;
	}
	
	fseek(manager->inputFP, sizeof(Record) * manager->current_input_file_positions[file_number], SEEK_SET);
	int remaining_records = manager->file_capacity[file_number] - manager->current_input_file_positions[file_number];

	int read_records;
	if (manager->input_buffer_capacity < remaining_records){
		read_records = manager->input_buffer_capacity;
		manager->current_input_file_positions[file_number] += read_records;
	} else {
		read_records = remaining_records;
		manager->current_input_file_positions[file_number] = -1;
	}

	int result = fread(manager->input_buffers[file_number], sizeof(Record), read_records, manager->inputFP);

	if (result <= 0){
		return FAILURE;
	}
	
	manager->total_input_buffer_elements[file_number] = read_records;
	manager->current_input_buffer_positions[file_number] = 0;
	
	fclose(manager->inputFP);
	
	return SUCCESS;
}

void clean_up (MergeManager * merger) {
	free(merger->heap);
	free(merger->input_file_numbers);
	free(merger->output_buffer);

	for (int i = 0; i < merger->heap_capacity; i++) { 
		free(merger->input_buffers[i]);
	}

	free(merger->input_buffers);
	free(merger->current_input_file_positions);
	free(merger->file_capacity);
}

int compare_heap_elements (HeapElement *a, HeapElement *b, int column) {
	if (column == 1){
		return (a->UID1 - b->UID1);
	}
	else{
		return (a->UID2 - b->UID2);
	}
}
