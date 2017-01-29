CC = gcc
CFLAGS = -O3 -Wall 
CFLAGS += -D_LARGEFILE_SOURCE
CFLAGS += -fno-exceptions
CFLAGS += -finline-functions
CFLAGS += -funroll-loops
CFLAGS += -D_FILE_OFFSET_BITS=64
 
# Source files
WRITE_BLOCKS_SRC=utils.c write_blocks_seq.c
WRITE_LINES_SRC=utils.c write_lines.c
READ_BLOCKS_SRC=utils.c read_blocks_seq.c
READ_RAM_SRC=utils.c read_ram_seq.c
READ_BLOCKS_RAND=utils.c read_blocks_rand.c
READ_RAM_RAND=utils.c read_ram_rand.c
WRITE_BLOCKS_RAND=utils.c write_blocks_rand.c
WRITE_RAM_RAND=utils.c write_ram_rand.c
 
# Binaries
all: write_blocks_seq write_lines read_blocks_seq read_ram_seq read_blocks_rand read_ram_rand write_blocks_rand write_ram_rand
 
#sequential writing in blocks
write_blocks_seq: $(WRITE_BLOCKS_SRC)
	$(CC) $(CFLAGS) $^ -o $@ 
 
clean:  
	rm write_blocks_seq write_lines read_blocks_seq read_ram_seq read_blocks_rand read_ram_rand write_blocks_rand write_ram_rand

dataclean:
	rm copy.csv records.dat
