#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct record {
  int uid1;
  int uid2;
} Record;

Record * make_record(char *str){
  char * val1 = strtok(str, ",");
  char * val2 = strtok(NULL, ",");
  int intval1 = atoi(val1);
  int intval2 = atoi(val2);
  
  Record * record = malloc(sizeof(Record));
  record->uid1 = intval1;
  record->uid2 = intval2;
  
  return record;
}

void read_and_buffer(char *file_name, int blocksize){
  char current_line[1024];
  FILE *fp_read;
  FILE *fp_write;
  int total_records = 0;
  int records_per_block = blocksize/sizeof(Record);
  Record * buffer = (Record*) calloc (records_per_block, sizeof(Record));
  
  if (!(fp_read = fopen(file_name, "r"))) {
      printf("Error: missing file.");
      exit(1);
  }
  
  if (!(fp_write = fopen("records.dat", "wb"))){
      printf("Error: could not open file for write.");
      exit(1);
  }
  
  while (fgets(current_line, 1024, fp_read) != NULL) {
    current_line[strcspn(current_line, "\r\n")] = '\0';
    Record * record = make_record(current_line);
    if (total_records < records_per_block ){
      memcpy(buffer + (total_records * sizeof(Record)), record, sizeof(Record));
      free(record);
      total_records += 1;
    } else {
      fwrite(buffer, sizeof(Record), total_records, fp_write);
      fflush(fp_write);
      total_records = 0;
    }
    
  }
  
  fclose(fp_write);
  free(buffer);
  fclose(fp_read);
}

