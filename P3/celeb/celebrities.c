#include "merge.h"
#include <sys/timeb.h>

int sort_file (char *filename, int total_mem, int block_size, int id );

int main(int argc, char **argv){

	// Timing statements
	struct timeb t_begin, t_end;
    long time_spent_ms = 0;

    // Begin timing
    ftime(&t_begin);

    // Program arguments
	char *filename = argv[1];
	int mem_size = atoi(argv[2]);
    int block_size = atoi(argv[3]);
	
	// Sort input file by uid1 and uid2 and store in output1.dat and output.dat respectively
    sort_file (filename, mem_size, block_size, 1);
    sort_file (filename, mem_size, block_size, 2);

    // Get indegree and outdegree
    

    // End timing statements
    ftime(&t_end);
    time_spent_ms = (long) (1000 *(t_end.time - t_begin.time) + (t_end.millitm - t_begin.millitm))/1000; 
    printf("Total time is:%lu\n", time_spent_ms);  

	return 0;
}
