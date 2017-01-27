#include "utils.h"

int main(int argc, char *argv[]){
  struct timeb start;
  struct timeb end;

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

  ftime(&start);
  while (fgets(current_line, 1024, fp_read) != NULL) {
    fprintf(fp_write, "%s", current_line);
  }

  fclose(fp_write);
  ftime(&end);
  fclose(fp_read);

  long time = (long) (1000 * (end.time - start.time) + (end.millitm - start.millitm));
  printf("Processing Rate: %.3f\n", (float)time/1000);

}
