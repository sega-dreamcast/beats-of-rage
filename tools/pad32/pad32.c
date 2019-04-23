#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  FILE *f;
  if(argc != 2) {
    printf("usage: %s file\n", argv[0]);
    return 1;
  }
  f = fopen(argv[1], "r+b"); if(!f) { perror(argv[1]); return 1; }
  fseek(f, 0, SEEK_END);
  while(ftell(f) & 31) fputc(0, f);
  fclose(f);
  return 0;
}
