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

	int records_per_block = block_size/sizeof(Record);
	Record * buffer = (Record *) calloc (records_per_block, sizeof(Record));

	int result = fread(buffer, sizeof(Record), records_per_block, fp_write);
	if (result != records_per_block){
		printf("Error: sizing issue.\n");
		exit(1);
	};

	int i = 0;
	ftime(&t_begin);
	while (i < x) {
		int random = rand() % records_per_block;
		i += 1;
		total_records++;
		//printf("random: %d\n", random);
		//printf("before replace - uid1:%d uid2:%d\n", buffer[random].uid1, buffer[random].uid2);
		buffer[random].uid1 = 1;
		buffer[random].uid2 = 2;
	//printf("after replace - uid1:%d uid2:%d\n", buffer[random].uid1, buffer[random].uid2);
	};
	ftime(&t_end);

  time_spent_ms = (long) (1000 *(t_end.time - t_begin.time) + (t_end.millitm - t_begin.millitm)); 
  printf ("Data rate: %.3f MBPS\n", ((total_records*sizeof(Record))/(float)time_spent_ms * 1000)/1000000);
	return 0;
}
