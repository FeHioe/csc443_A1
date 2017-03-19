C = gcc
CFLAGS = -O3 -Wall 
CFLAGS += -D_LARGEFILE_SOURCE
CFLAGS += -fno-exceptions
CFLAGS += -finline-functions
CFLAGS += -funroll-loops
CFLAGS += -D_FILE_OFFSET_BITS=64
CFLAGS += -std=c99
 
# Source files
DISK_SORT_SRC=disk_sort.c
SORTED_RUN_SRC=sort_file.c
MERGE_EXTERNAL_SRC=merge_external.c
CELEBRITIES_SRC = celebrities.c

# Binaries
all: celebrities
 
#sequential writing in blocks
celebrities: $(CELEBRITIES_SRC) $(DISK_SORT_SRC) $(SORTED_RUN_SRC) $(MERGE_EXTERNAL_SRC)
	$(CC) $(CFLAGS) $^ -o $@

clean:  
	rm sublist*.dat output*.dat celebrities indegree.dat outdegree.dat



