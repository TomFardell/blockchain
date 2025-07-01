#include <stdlib.h>
#include <stdio.h>
#include "bitmap.h"

static int full_bytes_needed(int num_bits) { return (num_bits / BYTE_SIZE) + ((num_bits % BYTE_SIZE) != 0); }

bitmap bitmap_init(int size) {
  int bytes_required = full_bytes_needed(size);
  unsigned char *map = calloc(bytes_required, 1);
  if (!map) {
    fprintf(stderr, "Failed to allocate memory for bitmap.\n");
    exit(EXIT_FAILURE);
  }

  return (bitmap){size, map};
}

// Get the bit in a position, either 0 or 1
int bitmap_get_bit(bitmap bmap, int index) {
  if (index < 0 || index >= bmap.size) {
    fprintf(stderr, "Index %d is out of range for bitmap of size %d.\n", index, bmap.size);
    exit(EXIT_FAILURE);
  }

  byte this_byte = bmap.map[index / BYTE_SIZE];
  // Shifting the 1 this way is consistent with setting bytes directly
  return (this_byte & (1 << (BYTE_SIZE - 1 - (index % BYTE_SIZE)))) != 0;
}

// Set the bit in a position to a new value
void bitmap_set_bit(bitmap bmap, int index, int new_value) {
  if (index < 0 || index >= bmap.size) {
    fprintf(stderr, "Index %d is out of range for bitmap of size %d.\n", index, bmap.size);
    exit(EXIT_FAILURE);
  }

  if (new_value != 0 && new_value != 1) {
    fprintf(stderr, "Can only set a bit to 0 or 1 (not %d).\n", new_value);
    exit(EXIT_FAILURE);
  }

  // If the bit is already set to this value, do nothing
  if (bitmap_get_bit(bmap, index) == new_value) return;

  // Otherwise toggle the bit
  bmap.map[index / BYTE_SIZE] ^= (1 << (BYTE_SIZE - 1 - (index % BYTE_SIZE)));
}

// Set the full byte. Indexing is determined on the byte level, i.e. a byte index of 2 corresponds to the byte
// starting at the 16th bit. If the bitmap's size is not a multiple of 8, we can still set the final byte, though
// we cannot access the trailing bits individually. Note we are not accessing any memory we didn't allocate
void bitmap_set_byte(bitmap bmap, int byte_index, byte new_value) {
  if (byte_index < 0 || byte_index >= full_bytes_needed(bmap.size)) {
    fprintf(stderr, "Byte index %d is out of range for bitmap of size %d (%d bytes).\n", byte_index, bmap.size,
            full_bytes_needed(bmap.size));
    exit(EXIT_FAILURE);
  }

  // Simply set the byte
  bmap.map[byte_index] = new_value;
}

void bitmap_print_bin(bitmap bmap) {
  for (int i = 0; i < bmap.size; i++) {
    printf("%d", bitmap_get_bit(bmap, i));
    if (i % BYTE_SIZE == BYTE_SIZE - 1) printf(" ");
  }
}

void bitmap_print_bin_on_line(bitmap bmap) {
  bitmap_print_bin(bmap);
  printf("\n");
}

// Note for non multiples of 8, the final entry assumes trailing zeros are present in the map
void bitmap_print_hex(bitmap bmap) {
  for (int i = 0; i < full_bytes_needed(bmap.size); i++) {
    printf("%02x ", bmap.map[i]);
  }
}

void bitmap_print_hex_on_line(bitmap bmap) {
  bitmap_print_hex(bmap);
  printf("\n");
}

void bitmap_print_den(bitmap bmap) {
  for (int i = 0; i < full_bytes_needed(bmap.size); i++) {
    printf("%03d ", bmap.map[i]);
  }
}

void bitmap_print_den_on_line(bitmap bmap) {
  bitmap_print_den(bmap);
  printf("\n");
}

void bitmap_free(bitmap bmap) {
  free(bmap.map);
  bmap.map = NULL;
}
