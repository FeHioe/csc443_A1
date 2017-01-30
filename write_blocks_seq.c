#include "utils.h"
#include <time.h>

int main(int argc, char *argv[]){
  struct timeb start;
  struct timeb end;

  time_t tstart, tend;
 
  char* filename = argv[1];
  int blocksize = atoi(argv[2]);
  
  char current_line[1024];
  FILE *fp_read;
  FILE *fp_write;

  int total_records = 0;
  int records_read = 0;
  int records_per_block = blocksize/sizeof(Record);
  Record * buffer = (Record*) calloc (records_per_block, sizeof(Record));
  
  if (!(fp_read = fopen(filename, "r"))) {
      printf("Error: could not open file for read.");
      exit(1);
  }
  if (!(fp_write = fopen("records.dat", "wb"))){
      printf("Error: could not open file for write.");
      exit(1);
  }

  //printf("before while loop\n");
  ftime(&start);
  time(&tstart);
  int i = 0;
  while (fgets(current_line, 1024, fp_read) != NULL) {
    current_line[strcspn(current_line, "\r\n")] = '\0';
    
    if (strlen(current_line) > 0){

   		// printf("before making record\n");
    	Record record = make_record(current_line);
    	printf("uid: %d, uid2: %d\n", record.uid1, record.uid2);
	
    	//printf("after making record, before if statement\n");

    	if (total_records < records_per_block){
      		//printf("right before memcpy\n");
      		buffer[i] = record;
	  		i += 1;

	  		printf("buf id1: %d buf id2: %d\n", buffer[i-1].uid1, buffer[i-1].uid2);
      
      		total_records += 1;
   	  		records_read += 1;
      		//printf("total records: %d\n", total_records);
    	} else {
      		fwrite(buffer, sizeof(Record), total_records, fp_write);
      		fflush(fp_write);
			memset(buffer, 0, blocksize);
      		total_records = 0;
    	}
	}
    
  }
  if (total_records > 0) {
	 fwrite(buffer, sizeof(Record), total_records, fp_write);
     fflush(fp_write);
     total_records = 0;
  }  

  fclose(fp_write);
  time(&tend);
  ftime(&end);
  free(buffer);
  fclose(fp_read);
  
  printf("%hu\n%hu\n", end.millitm, start.millitm);
  printf("%f\n", difftime(tend, tstart));
  printf("%d\n", records_read);
  long time = (long) (1000 * (end.time - start.time) + (end.millitm - start.millitm));
  printf("Processing Rate: %.3f\n", ((total_records * sizeof(Record))/(float)time * 1000)/1024*1024);
  
  return 0;
}

