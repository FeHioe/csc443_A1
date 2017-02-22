#include "merge.h"

//manager fields should be already initialized in the caller
int merge_runs (MergeManager * merger){	
	int  result; //stores SUCCESS/FAILURE returned at the end	
	
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

int init_merge (MergeManager * manager) {
	
	return SUCCESS;
}

int flush_output_buffer (MergeManager * manager) {
	if (fwrite(manager->output_buffer, sizeof(Record), manager->output_buffer_capacity, manager->outputFP) != manager->output_buffer_capacity) {
		printf("ERROR: output buffer could not be written to disk");
		return FAILURE;
	}
	fflush(manager->outputFP);
	current_output_buffer_position = 0;
	return SUCCESS;
}

int get_next_input_element(MergeManager * manager, int file_number, Record *result) {
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
  		fseek(fp_read, manager->current_input_filepositions[file_number], SEEK_SET);
  		num_elements_read = fread(manager->input_buffers[file_number], sizeof(Record), manager->input_buffer_capacity, fp_read);

  		if (num_elements_read < 1) {
  			fclose(fp_read);
  			manager->total_input_buffer_elements[file_number] = 0;
  			manager->current_input_file_positions[file_number] = -1;
  			manager->current_input_buffer_positions[file_number] = -1;
  			printf("file exhausted");
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
	
	}
	return SUCCESS;
}


int refill_buffer (MergeManager * manager, int file_number) {
	return SUCCESS;
}

void clean_up (MergeManager * merger) {
	fclose(manager->outputFP);
	free(manager->heap);
	free(manager->inputFP);
	free(manager->input_file_numbers);
	free(manager->output_buffer);
	for (int i = 0; i < manager->input_buffer_capacity; i++) {
		free(manager->input_buffers[i]);
	}
	free(manager->input_buffers);
	free(manager->current_input_file_positions);
	free(manager->current_input_buffer_positions);
	free(manager->total_input_buffer_elements);
	free(manager);
}
// if a bigger than b, return greater than 0
int compare_heap_elements (HeapElement *a, HeapElement *b) {
	return (a->UID2 - b->UID2);
}