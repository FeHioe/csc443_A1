#include "utils.h"

int main(int argc, char *argv[]){
  struct timeb t_begin, t_end;
  long time_spent_ms;
 
  char* filename = argv[1];
  int blocksize = atoi(argv[2]);
  
  char current_line[1024];
  FILE *fp_read;
  FILE *fp_write;

  long tr = 0;
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
  ftime(&t_begin);
  //int i = 0;
  while (fgets(current_line, 1024, fp_read) != NULL) {
  	 tr++;
    current_line[strcspn(current_line, "\r\n")] = '\0';
    
    if (strlen(current_line) > 0){

//   		 printf("before making record\n");
    	Record record = make_record(current_line);
    //	printf("uid: %d, uid2: %d\n", record.uid1, record.uid2);
	
    //	printf("after making record, before if statement\n");

    	if (total_records < records_per_block){
      //		printf("right before memcpy\n");
      		buffer[total_records] = record;
	  		//i += 1;
	  	//	printf("buf id1: %d buf id2: %d\n", buffer[total_records].uid1, buffer[total_records].uid2);
      
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
  ftime(&t_end);  
  free(buffer);
  fclose(fp_read);
  
  time_spent_ms = (long) (1000 *(t_end.time - t_begin.time) + (t_end.millitm - t_begin.millitm)); 
  printf ("Data rate: %.3f MBPS\n", ((tr*sizeof(Record))/(float)time_spent_ms * 1000)/1000000);
  
  return 0;
}

