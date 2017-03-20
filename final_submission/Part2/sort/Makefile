CC = gcc
CFLAGS = -O3 -Wall 
CFLAGS += -D_LARGEFILE_SOURCE
CFLAGS += -fno-exceptions
CFLAGS += -finline-functions
CFLAGS += -funroll-loops
CFLAGS += -D_FILE_OFFSET_BITS=64
 
# Source files
DISK_SORT_SRC= disk_sort.c merge_external.c main.c
DISTRIBUTION_SRC= distribution.c

# Binaries
all: disk_sort
 
#sequential writing in blocks
disk_sort: $(DISK_SORT_SRC)
	$(CC) $(CFLAGS) $^ -o $@ -lm -std=c99
distribution: $(DISTRIBUTION_SRC)
	$(CC) $(CFLAGS) $^ -o $@ -lm -std=c99

clean:  
	rm disk_sort sublist* output*

