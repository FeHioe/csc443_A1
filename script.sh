#!/bin/bash

make

filename=edges.csv
./write_blocks_seq $filename 512
./write_blocks_seq $filename 1024
./write_blocks_seq $filename 4*1024
./write_blocks_seq $filename 8*1024
./write_blocks_seq $filename 16*1024
./write_blocks_seq $filename 32*1024
./write_blocks_seq $filename 1*1024*1024
./write_blocks_seq $filename 2*1024*1024
./write_blocks_seq $filename 4*1024*1024
 
