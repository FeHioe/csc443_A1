#include "merge.h"
#include "celebrities.h"
#include <sys/timeb.h>

int sort_file(char *input_file, int mem_size, int block_size, int column);

int compare_heap (const void *a, const void *b) {
	JoinHeapElement join_a = *(JoinHeapElement*)a;
	JoinHeapElement join_b = *(JoinHeapElement*)b;
	return (join_a.diff - join_b.diff);
}

int print (JoinManager * manager){
	qsort(manager->heap, manager->current_heap_size, sizeof(JoinHeapElement), compare_heap);

	for (int i = manager->current_heap_size-1; i >= 0; i--){
		printf("UID=%d， indegree=%d, outdegree=%d, diff=%d\n",
			manager->heap[i].UID, manager->heap[i].indegree,
			manager->heap[i].outdegree, manager->heap[i].diff);
	}
	
	return SUCCESS;
}

void join_clean_up (JoinManager * merger) {
	free(merger->heap);
	free(merger->input_file_numbers);

	for (int i = 0; i < 2; i++) { 
		free(merger->input_buffers[i]);
	}

	free(merger->input_buffers);
	free(merger->current_input_file_positions);
	free(merger->elements);
}

int join_refill_buffer (JoinManager * manager, int file_number) {
	
	char filename[MAX_PATH_LENGTH];
	if (file_number == 0){
		strcpy(filename, "outdegree.dat");
	} else {
		strcpy(filename, "indegree.dat");
	}

	if (manager->current_input_file_positions[file_number] == -1){
		return EMPTY;
	}
	
	manager->inputFP = fopen(filename, "rb");
	if (!manager->inputFP){
		return FAILURE;
	}
	
	fseek(manager->inputFP, sizeof(Degree) * manager->current_input_file_positions[file_number], SEEK_SET);
	int remaining_records = manager->elements[file_number] - manager->current_input_file_positions[file_number];

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


int join_get_next_input_element(JoinManager * manager, int file_number, Degree *result) {
	
	if (manager->current_input_buffer_positions[file_number] == manager->total_input_buffer_elements[file_number]){
		if (manager->total_input_buffer_elements[file_number] < manager->input_buffer_capacity){
			result->UID = -1;
			return EMPTY;

		} else {	
			int refill = join_refill_buffer(manager, file_number);
			if (refill != SUCCESS){
				return refill;
			}
		}
	}
	
	*result = manager->input_buffers[file_number][manager->current_input_buffer_positions[file_number]];
	manager->current_input_buffer_positions[file_number]++;
	
	return SUCCESS;
}

int join_compare_heap_elements (JoinHeapElement *a, JoinHeapElement *b) {
	return (a->diff - b->diff);
}

int join_insert_into_heap (JoinManager * manager, int uid, int indegree, int outdegree, int diff){
	JoinHeapElement new_heap_element;
	int child, parent;
	
	new_heap_element.UID = uid;
	new_heap_element.indegree = indegree;
	new_heap_element.outdegree = outdegree;
	new_heap_element.diff = diff;
	
	if (manager->current_heap_size == manager->heap_capacity) {
		printf( "Unexpected ERROR: heap is full\n");
		return FAILURE;
	}
  	
	child = manager->current_heap_size++; /* the next available slot in the heap */
	
	while (child > 0) {
		parent = (child - 1) / 2;
		if (join_compare_heap_elements(&(manager->heap[parent]), &new_heap_element) > 0) {
			manager->heap[child] = manager->heap[parent];
			child = parent;
		} 
		else 
			break;
	}
	
	manager->heap[child]= new_heap_element;

	return SUCCESS;
}


int join_pop (JoinManager * manager){

	JoinHeapElement temp;
	int child;
	int parent = 0;

	if(manager->current_heap_size == 0){
		printf( "Unexpected ERROR: heap is empty\n");
		return FAILURE;
	}

	temp = manager->heap[--manager->current_heap_size];

	while ((child = (2 * parent) + 1) < manager->current_heap_size) {
		if (child + 1 < manager->current_heap_size && 
			(join_compare_heap_elements(&(manager->heap[child]), &(manager->heap[child + 1]))>0)){
			++child;
		}
		
		if (join_compare_heap_elements(&temp, &(manager->heap[child]))>0) {
			manager->heap[parent] = manager->heap[child];
			parent = child;
		} 
		else 
			break;
	}

	manager->heap[parent] = temp;
	
	return SUCCESS;
}

int iterate (JoinManager * manager, Degree * given_out, Degree * given_in){
	int result1, result2;
	int uid, indegree, outdegree, diff;

	if (given_out->UID == -1 && given_in->UID == -1){
		return EMPTY;

	} else if (given_out->UID == -1){
		uid = given_in->UID;
		indegree = given_in->count;
		outdegree = 0;
		result2 = join_get_next_input_element (manager, 1, given_in);
		if (result2 == FAILURE){
			return result2;
		}

	} else if (given_in->UID == -1){
		uid = given_out->UID;
		indegree = 0;
		outdegree = given_out->count;
		result1 = join_get_next_input_element (manager, 0, given_out);
		if (result1 == FAILURE){
			return result1;
		}

	} else{

		if (given_in->UID == given_out->UID){
			uid = given_in->UID;
			indegree = given_in->count;
			outdegree = given_out->count;
			result2 = join_get_next_input_element (manager, 1, given_in);
			result1 = join_get_next_input_element (manager, 0, given_out);
			if (result1 == FAILURE){
				return result1;
			}
			if (result2 == FAILURE){
				return result2;
			}

		} else if (given_out->UID < given_in->UID){
			uid = given_out->UID;
			indegree = 0;
			outdegree = given_out->count;
			result1 = join_get_next_input_element (manager, 0, given_out);
			if (result1 == FAILURE){
				return result1;
			}

		} else{
			uid = given_in->UID;
			indegree = given_in->count;
			outdegree = 0;
			result2 = join_get_next_input_element (manager, 1, given_in);
			if (result2 == FAILURE){
				return result2;
			}
		}
	}

	diff = indegree - outdegree;

	int result;
	if (manager->current_heap_size < manager->heap_capacity) {
		result = join_insert_into_heap (manager, uid, indegree, outdegree, diff);
		if (result != SUCCESS){
			return FAILURE;
		}

	} else {
		if (manager->heap[0].diff < diff){
			if (join_pop(manager) != SUCCESS){
				return FAILURE;
			}

			result = join_insert_into_heap (manager, uid, indegree, outdegree, diff);
			if (result != SUCCESS){
				return FAILURE;
			}
		}
	}
	
	return SUCCESS;
}


int get_degrees(char* filename, int mem_size, int block_size, int column){
	FILE *fp_read;
	FILE *fp_write;
	
    Record* record_buffer = (Record *) calloc (block_size / sizeof(Record) * (mem_size / block_size-1), sizeof(Record)) ;
	Degree* count_buffer = (Degree *) calloc (block_size / sizeof(Degree), sizeof(Degree)) ;
	
    if (!(fp_read = fopen ( filename , "rb" ))){
		exit(1);
	}
	
    fseek(fp_read, 0, SEEK_END);
    int file_size = ftell(fp_read);
    fseek(fp_read, 0, SEEK_SET); 

    int all_records = file_size / sizeof(Record);
	
	char *output_file;
	if (column == 1){
		output_file = "outdegree.dat";
	}
	else{
		output_file = "indegree.dat";
	}
	
	if (!(fp_write = fopen ( output_file , "wb" ))){
		exit(1);
	}
	
	/*compute the query*/
	int count = 0;
	int current_uid = -1;
   	int input_buffer_size = (mem_size / block_size - 1) * block_size / sizeof(Record);
	int count_i = 0;

    while (fread(record_buffer, sizeof(Record), input_buffer_size, fp_read) > 0){

		int remaining_records = all_records - input_buffer_size;
		int read_records;
		if(0 <= remaining_records){
			all_records = all_records - input_buffer_size;
			read_records = input_buffer_size;
		} else {
			read_records = all_records;
		}
		
		int i;
		for (i = 0; i < read_records; i++){
			if (column == 1){
				if (current_uid == -1){
					count = 1;
					current_uid = record_buffer[i].UID1;
				} else if(record_buffer[i].UID1 == current_uid) {
					count += 1;
				} else {
					count_buffer[count_i].UID = current_uid;
					count_buffer[count_i].count = count;
					
					count_i++;
					
					if (block_size / sizeof(Degree) <= count_i ){
						fwrite (count_buffer, sizeof(Degree), count_i, fp_write);
						count_i = 0;
					}
					
					count = 1;
					current_uid = record_buffer[i].UID1;
				}
			}else{
				if (current_uid == -1){
					count = 1;
					current_uid = record_buffer[i].UID2;
				} else if(record_buffer[i].UID2 == current_uid) {
					count += 1;
				} else {
					count_buffer[count_i].UID = current_uid;
					count_buffer[count_i].count = count;
					
					count_i++;
					
					if (block_size / sizeof(Degree) <= count_i){
						fwrite (count_buffer, sizeof(Degree), count_i, fp_write);
						count_i = 0;
					}
					
					count = 1;
					current_uid = record_buffer[i].UID2;
				}
			}
		}
	}
	
	count_buffer[count_i].UID = current_uid;
	count_buffer[count_i].count = count;
	count_i++;
	fwrite(count_buffer, sizeof(Degree), count_i, fp_write);

	fclose(fp_read);
	free(record_buffer);
	free(count_buffer);
	fclose(fp_write);
	
	return 0;
}

int init_join (JoinManager * manager, int mem_size, int block_size) {

	int buffer_capacity = (mem_size / 2 / block_size) * (block_size / sizeof(Degree));
	int sublist_num = 2;

	manager->current_heap_size = 0;
	manager->heap_capacity = 10;
	manager->heap = (JoinHeapElement *) calloc (manager->heap_capacity, sizeof (JoinHeapElement));
	
	manager->input_file_numbers = (int *) calloc (sublist_num, sizeof (int));
	for (int i = 0; i < sublist_num; i++){
		manager->input_file_numbers[i] = i;
	}
	
	manager->input_buffers = (Degree **) calloc (sublist_num, sizeof (Degree *));
	for (int i = 0; i < sublist_num; i++) { 
		manager->input_buffers[i] = (Degree *) calloc (buffer_capacity, sizeof (Degree));
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

	strcpy(manager->output_file_name, "celebrities.dat");

	manager->elements = (int *) calloc (sublist_num, sizeof (int));
	for (int i = 0; i < sublist_num; i++){

		char filename[MAX_PATH_LENGTH];
		if (i == 0){
			strcpy(filename, "outdegree.dat");
		} else {
			strcpy(filename, "indegree.dat");
		}

		FILE *fp_read;
		fp_read = fopen(filename, "rb");
		if (!fp_read){
			return FAILURE;
		}
	
		fseek(fp_read, 0, SEEK_END);
		int filesize = ftell(fp_read);
		int all_records = filesize / sizeof(Degree);
		fseek(fp_read, 0, SEEK_SET); 
	
		fclose(fp_read);

		manager->elements[i] = all_records;
	}
	
	for (int i = 0; i < sublist_num; i++){
		join_refill_buffer(manager, i);
	}
	
	return SUCCESS;
}

int query (JoinManager * manager, int mem_size, int block_size){	
	int result; //stores SUCCESS/FAILURE returned at the end
	
	Degree out;
	out.UID = -1;
	out.count = -1;

	Degree in; 
	in.UID = -1;
	in.count = -1;
	
	//1. go in the loop through all input files and fill-in initial buffers
	if (init_join (manager, mem_size, block_size)!=SUCCESS)
		return FAILURE;
	
	int out_result = join_get_next_input_element(manager, 0, &out);
	int in_result = join_get_next_input_element(manager, 1, &in);

	result = in_result || out_result;
	
	if ((out_result==FAILURE) || (in_result == FAILURE)){
		return FAILURE;
	} 
	
	while (result == 0) { 
		result = iterate(manager, &out, &in);
		if (result == FAILURE){
			return FAILURE;
		}
	}
	
	print(manager);
	join_clean_up(manager);
	return SUCCESS;
}


int main(int argc, char **argv){

	struct timeb t_begin, t_end;
    long time_spent_ms = 0;

    ftime(&t_begin);

	JoinManager manager;
	int MB = 1024*1024;
	char *filename = argv[1];
	int mem_size = atoi(argv[2]) * MB;
    int block_size = atoi(argv[3]) * MB;
	
	sort_file(filename, mem_size, block_size, 1);
	sort_file(filename, mem_size, block_size, 2);
	
	get_degrees("output1.dat", mem_size, block_size, 1);
	get_degrees("output2.dat", mem_size, block_size, 2);

	int result = query (&manager, mem_size, block_size);

    ftime(&t_end);

    time_spent_ms = (long) (1000 *(t_end.time - t_begin.time) + (t_end.millitm - t_begin.millitm))/1000; 
    printf("Total time:%lu\n", time_spent_ms);  
	return result;
	
}
