#include "utils.h"

Record make_record(char *str){

  //printf("this should print %s\n", str);
  if (str[0] == '\0') {
	printf("empty string\n");
  }

  char * val1 = strtok(str, ",");
  //printf("after filling val 1\n");
  char * val2 = strtok(NULL, ",");
  //printf("after filling val 2, val: %s\n", val2);

  Record record;
  //printf("making the record after this\n");
  record.uid1 = atoi(val1);
  record.uid2 = atoi(val2);
  
  return record;
}
