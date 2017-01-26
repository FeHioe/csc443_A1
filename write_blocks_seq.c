#include "utils.c"

int main(int argc, char *argv[]){
  char* filename = argv[1];
  int blocksize = atoi(argv[2]);
  
  read_and_buffer(filename, blocksize);
}

