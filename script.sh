#!/bin/bash

make

filename=./data/edges.csv
echo "first experiment"
./write_blocks_seq $filename 512
./write_blocks_seq $filename 1024
./write_blocks_seq $filename 4096
./write_blocks_seq $filename 8192
./write_blocks_seq $filename 16384
./write_blocks_seq $filename 32768
./write_blocks_seq $filename 1048576
./write_blocks_seq $filename 2097152
./write_blocks_seq $filename 4194304
 
cp $filename ./cp1.csv
filename=cp1.csv
echo "second experiment"
./write_blocks_seq $filename 512
./write_blocks_seq $filename 1024
./write_blocks_seq $filename 4096
./write_blocks_seq $filename 8192
./write_blocks_seq $filename 16384
./write_blocks_seq $filename 32768
./write_blocks_seq $filename 1048576
./write_blocks_seq $filename 2097152
./write_blocks_seq $filename 4194304

cp $filename ./cp2.csv
filename=cp2.csv
echo "third experiment"
./write_blocks_seq $filename 512
./write_blocks_seq $filename 1024
./write_blocks_seq $filename 4096
./write_blocks_seq $filename 8192
./write_blocks_seq $filename 16384
./write_blocks_seq $filename 32768
./write_blocks_seq $filename 1048576
./write_blocks_seq $filename 2097152
./write_blocks_seq $filename 4194304

cp $filename ./cp3.csv
filename=cp3.csv
echo "fourth experiment"
./write_blocks_seq $filename 512
./write_blocks_seq $filename 1024
./write_blocks_seq $filename 4096
./write_blocks_seq $filename 8192
./write_blocks_seq $filename 16384
./write_blocks_seq $filename 32768
./write_blocks_seq $filename 1048576
./write_blocks_seq $filename 2097152
./write_blocks_seq $filename 4194304

cp $filename ./cp4.csv
filename=cp4.csv
echo "fifth experiment"
./write_blocks_seq $filename 512
./write_blocks_seq $filename 1024
./write_blocks_seq $filename 4096
./write_blocks_seq $filename 8192
./write_blocks_seq $filename 16384
./write_blocks_seq $filename 32768
./write_blocks_seq $filename 1048576
./write_blocks_seq $filename 2097152
./write_blocks_seq $filename 4194304



