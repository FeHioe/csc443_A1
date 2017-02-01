#include "utils.h"

int main(int argc, char *argv[]){
   struct timeb t_begin, t_end;
   long time_spent_ms;
   long total_records = 0;
   
   char* filename = argv[1];
   char current_line[1024];
	FILE * fp_read;
	FILE * fp_write;

	if (!(fp_read = fopen(filename, "r"))) {
    	printf("Error: could not open file for read.");
    	exit(1);
    }
    if (!(fp_write = fopen("copy.csv", "wb"))){
    	printf("Error: could not open file for write.");
    	exit(1);
	}

  ftime(&t_begin);
  while (fgets(current_line, 1024, fp_read) != NULL) {
    fprintf(fp_write, "%s", current_line);
    total_records++;
  }

  fclose(fp_write);
  ftime(&t_end);
  fclose(fp_read);

  time_spent_ms = (long) (1000 *(t_end.time - t_begin.time)
       + (t_end.millitm - t_begin.millitm)); 
  printf ("Data rate: %.3f MBPS\n", ((total_records*sizeof(Record))/(float)time_spent_ms * 1000)/(1024*1024));
 
  return 0;  
}
