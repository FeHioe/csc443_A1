#ifndef DISK_SORT_H
#define DISK_SORT_H

#include "merge.h"

int compare (const void *a, const void *b);
void sort_array_by_uid2(Record * buffer, int total_records);
int phase1(char *filename, int total_mem, int block_size);
#endif
