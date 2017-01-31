#include "utils.h"

int main(int argc, char *argv[]){
	char *filename = argv[1];
	int block_size = atoi(argv[2]);
	int x = atoi(argv[3]);

   struct timeb t_begin, t_end;
   long time_spent_ms;
   long total_records = 0;

	FILE *fp_write;
	if (!(fp_write = fopen(filename, "rb+"))) {
    	printf("Error: could not open file for write.\n");
    	exit(1);
    };

	int records_in_block = block_size/sizeof(Record); 

	Record * buffer = (Record *) calloc (1, sizeof(Record));
	int i = 0;
	ftime(&t_begin);
	while (i < x) {
		int random = rand() % records_in_block;
		total_records++;

		//printf("random: %d\n", random);
		i += 1;

		fseek(fp_write, random*sizeof(Record), SEEK_SET);
		buffer[0].uid1 = 1;
		buffer[0].uid2 = 2;
		fwrite(buffer, sizeof(Record), 1, fp_write);
		fflush(fp_write);
	};
	ftime(&t_end);

	  time_spent_ms = (long) (1000 *(t_end.time - t_begin.time) + (t_end.millitm - t_begin.millitm)); 
  printf ("Data rate: %.3f MBPS\n", ((total_records*sizeof(Record))/(float)time_spent_ms * 1000)/1000000);
	return 0;
}
