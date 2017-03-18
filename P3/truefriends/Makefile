CC = gcc
CFLAGS = -O3 -Wall 
CFLAGS += -D_LARGEFILE_SOURCE
CFLAGS += -fno-exceptions
CFLAGS += -finline-functions
CFLAGS += -funroll-loops
CFLAGS += -D_FILE_OFFSET_BITS=64
 
# Source files
HEADERS = merge.h disk_sort.h
DISK_SORT_OBJ = disk_sort.o merge_external.o true_friends.o

 
#sequential writing in blocks
true_friends: $(DISK_SORT_OBJ) 
	$(CC) $(CFLAGS) $^ -o $@ -lm -std=c99
%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:  
	rm disk_sort distribution

