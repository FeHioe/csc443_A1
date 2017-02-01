#!/bin/bash

make

filename=./data/edges.csv
./write_blocks_seq $filename 512
./write_blocks_seq $filename 1024
./write_blocks_seq $filename 4096
./write_blocks_seq $filename 8192
./write_blocks_seq $filename 16384
./write_blocks_seq $filename 32768
./write_blocks_seq $filename 1048576
./write_blocks_seq $filename 2097152
./write_blocks_seq $filename 4194304
 
