
typedef struct degree {
	int UID;
	int count;
} Degree;


typedef struct JoinHeapElement {
	int UID;
	int indegree;
	int outdegree;
	int diff;
} JoinHeapElement;


typedef struct join_manager {
	FILE *inputFP; //stays closed, opens each time we need to reupload some amount of data from disk runs
	int *input_file_numbers;  //we need to know the run id to read from the corresponding run	
	Degree **input_buffers; //array of buffers to buffer part of runs
	int input_buffer_capacity; //how many elements max can each input buffer hold
	int *current_input_file_positions; //current position in each sorted run, can use -1 if the run is complete
	int *current_input_buffer_positions; //position in current input buffer
	int *total_input_buffer_elements;  //number of actual elements currently in input buffer - can be less than max capacity
	char output_file_name [MAX_PATH_LENGTH]; //stores name of the file to which to write the final output

	int *elements;

	JoinHeapElement *heap;		// A min heap that stores the top 10 celebrities.
	int current_heap_size;	// Number of celebrities in the heap
	int heap_capacity;	// Max number of celebrities that heap can contain
}JoinManager;
