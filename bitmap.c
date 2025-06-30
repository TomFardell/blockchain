#include <stdlib.h>
#include <stdio.h>

#define BYTE_SIZE 8

typedef unsigned char byte;

typedef struct bitmap {
  int size;
  byte *map;
} bitmap;

int full_bytes_needed(const int num_bits) { return (num_bits / BYTE_SIZE) + ((num_bits % BYTE_SIZE) != 0); }

bitmap bitmap_init(const int size) {
  int bytes_required = full_bytes_needed(size);
  unsigned char *map = calloc(bytes_required, 1);
  if (!map) {
    fprintf(stderr, "Failed to allocate memory for bitmap.");
    exit(EXIT_FAILURE);
  }

  return (bitmap){size, map};
}

void bitmap_print_hex(const bitmap bmap) {
  for (int i = 0; i < full_bytes_needed(bmap.size); i++) {
    printf("%x ", bmap.map[i]);
  }
}

void bitmap_delete(bitmap bmap) {
  free(bmap.map);
  bmap.map = NULL;
}

int main() {
  bitmap bmap = bitmap_init(25);
  bitmap_print_hex(bmap);
  printf("\n");
  bitmap_delete(bmap);

  return 0;
}
