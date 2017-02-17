CC = gcc
CFLAGS = -O3 -Wall 
CFLAGS += -D_LARGEFILE_SOURCE
CFLAGS += -fno-exceptions
CFLAGS += -finline-functions
CFLAGS += -funroll-loops
CFLAGS += -D_FILE_OFFSET_BITS=64
 
# Source files
DISK_SORT_SRC=merge.h disk_sort.c
MAIN_SRC=merge.h merge_external.c main.c

# Binaries
all: disk_sort main_merge
 
#sequential writing in blocks
disk_sort: $(DISK_SORT_SRC)
	$(CC) $(CFLAGS) $^ -o $@ -lm
main_merge: $(MAIN_SRC)
	$(CC) $(CFLAGS) $^ -o $@ -lm

clean:  
	rm disk_sort main_merge

