CC = gcc
CFLAGS = -O3 -Wall 
CFLAGS += -D_LARGEFILE_SOURCE
CFLAGS += -fno-exceptions
CFLAGS += -finline-functions
CFLAGS += -funroll-loops
CFLAGS += -D_FILE_OFFSET_BITS=64
 
# Source files
MAIN_SRC=merge.h merge_external.c sort_file.c disk_sort.c celebrities.c celebrities.h

# Binaries
all: main_merge
 
#sequential writing in blocks
main_merge: $(MAIN_SRC)
	$(CC) $(CFLAGS) $^ -o $@ -lm

clean:  
	rm disk_sort

clean_dat:
	rm output.dat output1.dat outdegree.dat indegree.dat
