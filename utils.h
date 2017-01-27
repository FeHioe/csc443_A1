#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/timeb.h>

typedef struct record {
  int uid1;
  int uid2;
} Record;

Record * make_record(char *str);
