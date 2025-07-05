#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bitmap.h"

bitmap bitmap_init_zeros(int size) {
  if (size < 0) {
    fprintf(stderr, "Bitmap cannot be initialised with size %d.\n", size);
    exit(EXIT_FAILURE);
  }

  int bytes_required = _full_bytes_needed(size);
  byte *map = calloc(bytes_required, sizeof *map);
  if (!map) {
    fprintf(stderr, "Failed to allocate memory for bitmap.\n");
    exit(EXIT_FAILURE);
  }

  return (bitmap){size, map};
}

bitmap bitmap_init_string(const char *string) {
  bitmap result = bitmap_init_zeros(strlen(string));

  for (int i = 0; i < result.size; i++) {
    if (string[i] != '0' && string[i] != '1') {
      fprintf(stderr, "The character '%c' (position %d of \"%s\") is invalid for bitmap input.\n", string[i], i,
              string);
      bitmap_free(&result);
      exit(EXIT_FAILURE);
    }

    bitmap_set_bit(&result, i, (string[i] == '1'));
  }

  return result;
}

// Get the bitmap corresponding to a specific number fit into a specified number of bytes
bitmap bitmap_init_number(u64 number, int bytes) {
  if (bytes < 0) {
    fprintf(stderr, "Bitmap cannot be initialised with size %d bytes.\n", bytes);
    exit(EXIT_FAILURE);
  }

  bitmap result = bitmap_init_zeros(bytes * BYTE_SIZE);
  bitmap_set_bytes_from_number(&result, number, 0, bytes);

  return result;
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
void bitmap_set_bit(bitmap *bmap, int index, int new_value) {
  if (index < 0 || index >= bmap->size) {
    fprintf(stderr, "Index %d is out of range for bitmap of size %d.\n", index, bmap->size);
    exit(EXIT_FAILURE);
  }

  if (new_value != 0 && new_value != 1) {
    fprintf(stderr, "Can only set a bit to 0 or 1 (not %d).\n", new_value);
    exit(EXIT_FAILURE);
  }

  // If the bit is already set to this value, do nothing
  if (bitmap_get_bit(*bmap, index) == new_value) return;

  // Otherwise toggle the bit
  bmap->map[index / BYTE_SIZE] ^= (1 << (BYTE_SIZE - 1 - (index % BYTE_SIZE)));
}

// Set the full byte. Indexing is determined on the byte level, i.e. a byte index of 2 corresponds to the byte
// starting at the 16th bit. If the bitmap's size is not a multiple of 8, we can still set the final byte, though
// we cannot access the trailing bits individually. Note we are not accessing any memory we didn't allocate
void bitmap_set_byte(bitmap *bmap, int byte_index, byte new_value) {
  if (byte_index < 0 || byte_index >= _full_bytes_needed(bmap->size)) {
    fprintf(stderr, "Byte index %d is out of range for bitmap of size %d (%d bytes).\n", byte_index, bmap->size,
            _full_bytes_needed(bmap->size));
    exit(EXIT_FAILURE);
  }

  // Simply set the byte
  bmap->map[byte_index] = new_value;
}

void bitmap_set_bytes_from_number(bitmap *bmap, u64 number, int starting_byte, int num_bytes) {
  // These are just for the error checking
  int starting_bit = starting_byte * BYTE_SIZE;
  int num_bits = num_bytes * BYTE_SIZE;
  if (starting_bit < 0 || starting_bit + num_bits > bmap->size || num_bits < 0) {
    fprintf(stderr, "Bytes [%d:%d] (bits [%d:%d]) are out of range for bitmap of size %d.\n", starting_byte,
            starting_byte + num_bytes, starting_bit, starting_bit + num_bits, bmap->size);
    exit(EXIT_FAILURE);
  }

  // Iterate through 00...00ff, 00...ff00, ..., ff...0000 and enumerate in i
  for (u64 window = 255, i = 0; i < num_bytes; window <<= BYTE_SIZE, i++) {
    byte this_byte = (number & window) >> (BYTE_SIZE * i);  // The ith digit of the number in base 256
    bitmap_set_byte(bmap, starting_byte + num_bytes - i - 1, this_byte);
  }
}

int bitmap_equal(bitmap bmap1, bitmap bmap2) {
  if (bmap1.size != bmap2.size) return 0;

  // Check bytes first for speed
  for (int i = 0; i < bmap1.size / BYTE_SIZE; i++) {
    if (bmap1.map[i] != bmap2.map[i]) return 0;
  }

  // Check only the necessary bits in the trailing byte
  for (int i = BYTE_SIZE * (bmap1.size / BYTE_SIZE); i < bmap1.size; i++) {
    if (bitmap_get_bit(bmap1, i) != bitmap_get_bit(bmap2, i)) return 0;
  }

  return 1;
}

bitmap bitmap_not(bitmap bmap) {
  bitmap result = bitmap_init_zeros(bmap.size);

  // Invert each byte individually
  for (int i = 0; i < bmap.size / BYTE_SIZE; i++) {
    bitmap_set_byte(&result, i, ~bmap.map[i]);
  }

  // Then invert bits in the trailing byte
  for (int i = BYTE_SIZE * (bmap.size / BYTE_SIZE); i < bmap.size; i++) {
    bitmap_set_bit(&result, i, 1 - bitmap_get_bit(bmap, i));
  }

  return result;
}

bitmap _bitmap_dual_operator(bitmap bmap1, bitmap bmap2, DualOperator operation) {
  if (bmap1.size != bmap2.size) {
    fprintf(stderr, "Cannot perform operation on differently sized bitmaps (%d and %d).\n", bmap1.size,
            bmap2.size);
  }

  bitmap result = bitmap_init_zeros(bmap1.size);

  // Do the operation on bytes first
  for (int i = 0; i < bmap1.size / BYTE_SIZE; i++) {
    switch (operation) {
      case OR:
        bitmap_set_byte(&result, i, bmap1.map[i] | bmap2.map[i]);
        break;
      case AND:
        bitmap_set_byte(&result, i, bmap1.map[i] & bmap2.map[i]);
        break;
      case XOR:
        bitmap_set_byte(&result, i, bmap1.map[i] ^ bmap2.map[i]);
        break;
    }
  }

  // Then on the trailing bits
  for (int i = BYTE_SIZE * (bmap1.size / BYTE_SIZE); i < bmap1.size; i++) {
    switch (operation) {
      case OR:
        bitmap_set_bit(&result, i, bitmap_get_bit(bmap1, i) | bitmap_get_bit(bmap2, i));
        break;
      case AND:
        bitmap_set_bit(&result, i, bitmap_get_bit(bmap1, i) & bitmap_get_bit(bmap2, i));
        break;
      case XOR:
        bitmap_set_bit(&result, i, bitmap_get_bit(bmap1, i) ^ bitmap_get_bit(bmap2, i));
        break;
    }
  }

  return result;
}

bitmap bitmap_or(bitmap bmap1, bitmap bmap2) { return _bitmap_dual_operator(bmap1, bmap2, OR); }
bitmap bitmap_and(bitmap bmap1, bitmap bmap2) { return _bitmap_dual_operator(bmap1, bmap2, AND); }
bitmap bitmap_xor(bitmap bmap1, bitmap bmap2) { return _bitmap_dual_operator(bmap1, bmap2, XOR); }

bitmap bitmap_lshift(bitmap bmap, int count) {
  // Handle negative counts by just passsing to the other method
  if (count < 0) return bitmap_rshift(bmap, -count);

  bitmap result = bitmap_init_zeros(bmap.size);

  for (int i = count; i < bmap.size; i++) {
    bitmap_set_bit(&result, i - count, bitmap_get_bit(bmap, i));
  }

  return result;
}

bitmap bitmap_rshift(bitmap bmap, int count) {
  if (count < 0) return bitmap_lshift(bmap, -count);

  bitmap result = bitmap_init_zeros(bmap.size);

  for (int i = count; i < bmap.size; i++) {
    bitmap_set_bit(&result, i, bitmap_get_bit(bmap, i - count));
  }

  return result;
}

bitmap bitmap_lrotate(bitmap bmap, int count) {
  if (count < 0) return bitmap_rrotate(bmap, -count);

  count = count % bmap.size;  // This method requires count to be between 0 and the size of the bitmap

  bitmap lshifted = bitmap_lshift(bmap, count);              // Bits shifted left
  bitmap rshifted = bitmap_rshift(bmap, bmap.size - count);  // Wrapped around bits

  bitmap result = bitmap_or(lshifted, rshifted);  // Get both sets of bits

  bitmap_free(&lshifted);
  bitmap_free(&rshifted);

  return result;
}

bitmap bitmap_rrotate(bitmap bmap, int count) {
  if (count < 0) return bitmap_lrotate(bmap, -count);

  count = count % bmap.size;  // This method requires count to be between 0 and the size of the bitmap

  bitmap rshifted = bitmap_rshift(bmap, count);              // Bits shifted right
  bitmap lshifted = bitmap_lshift(bmap, bmap.size - count);  // Wrapped around bits

  bitmap result = bitmap_or(lshifted, rshifted);  // Get both sets of bits

  bitmap_free(&lshifted);
  bitmap_free(&rshifted);

  return result;
}

// Get the sub-bitmap between start_index (inclusive) and end_index (exclusive)
bitmap bitmap_slice(bitmap bmap, int start_index, int end_index) {
  if (start_index < 0 || end_index > bmap.size || end_index < start_index) {
    fprintf(stderr, "Slice [%d:%d] is invalid for bitmap of size %d.\n", start_index, end_index, bmap.size);
    exit(EXIT_FAILURE);
  }

  bitmap result = bitmap_init_zeros(end_index - start_index);

  for (int i = start_index; i < end_index; i++) {
    bitmap_set_bit(&result, i - start_index, bitmap_get_bit(bmap, i));
  }

  return result;
}

// Select bits from bmap1 and bmap2
// choices[i] == 1 -> Take bmap1[i]
// choices[i] == 0 -> Take bmap2[i]
bitmap bitmap_choose(bitmap choices, bitmap bmap1, bitmap bmap2) {
  if (choices.size != bmap1.size || choices.size != bmap2.size) {
    fprintf(stderr, "The three bitmaps must be of the same size (not %d, %d and %d).\n", choices.size, bmap1.size,
            bmap2.size);
    exit(EXIT_FAILURE);
  }

  bitmap result = bitmap_init_zeros(choices.size);
  for (int i = 0; i < choices.size; i++) {
    switch (bitmap_get_bit(choices, i)) {
      case 1:
        bitmap_set_bit(&result, i, bitmap_get_bit(bmap1, i));
        break;
      case 0:
        bitmap_set_bit(&result, i, bitmap_get_bit(bmap2, i));
        break;
    }
  }

  return result;
}

// Get the resultant bitmap from choosing the majority bit at each index
bitmap bitmap_majority(bitmap bmap1, bitmap bmap2, bitmap bmap3) {
  if (bmap1.size != bmap2.size || bmap1.size != bmap3.size) {
    fprintf(stderr, "The three bitmaps must be of the same size (not %d, %d and %d).\n", bmap1.size, bmap2.size,
            bmap3.size);
    exit(EXIT_FAILURE);
  }

  bitmap result = bitmap_init_zeros(bmap1.size);

  for (int i = 0; i < bmap1.size; i++) {
    if (bitmap_get_bit(bmap1, i) + bitmap_get_bit(bmap2, i) + bitmap_get_bit(bmap3, i) > 1) {
      bitmap_set_bit(&result, i, 1);
    }
  }

  return result;
}

// Add two bitmaps of equal size, truncating the result to have the same size
bitmap bitmap_add_mod(bitmap bmap1, bitmap bmap2) {
  if (bmap1.size != bmap2.size) {
    fprintf(stderr, "Can only add two bitmaps of the same size (not %d and %d).\n", bmap1.size, bmap2.size);
    exit(EXIT_FAILURE);
  }

  bitmap result = bitmap_init_zeros(bmap1.size);
  int carry = 0;

  // First add individual bits on the trailing byte
  for (int i = bmap1.size - 1; i >= BYTE_SIZE * (bmap1.size / BYTE_SIZE); i--) {
    int b1 = bitmap_get_bit(bmap1, i);
    int b2 = bitmap_get_bit(bmap2, i);

    bitmap_set_bit(&result, i, b1 ^ b2 ^ carry);
    carry = b1 & b2;
  }

  // For the fully used bytes, do addition on each byte at a time
  for (int i = bmap1.size / BYTE_SIZE; i >= 0; i--) {
    int b1 = bmap1.map[i];
    int b2 = bmap2.map[i];

    bitmap_set_byte(&result, i, (b1 + b2 + carry) % BYTE_COMBINATIONS);
    carry = ((b1 + b2) >= BYTE_COMBINATIONS);
  }

  return result;
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
  for (int i = 0; i < _full_bytes_needed(bmap.size); i++) {
    printf("%02x ", bmap.map[i]);
  }
}

void bitmap_print_hex_on_line(bitmap bmap) {
  bitmap_print_hex(bmap);
  printf("\n");
}

void bitmap_print_den(bitmap bmap) {
  for (int i = 0; i < _full_bytes_needed(bmap.size); i++) {
    printf("%03d ", bmap.map[i]);
  }
}

void bitmap_print_den_on_line(bitmap bmap) {
  bitmap_print_den(bmap);
  printf("\n");
}

void bitmap_free(bitmap *bmap) {
  if (bmap->map == NULL) return;
  free(bmap->map);
  bmap->map = NULL;
  bmap = NULL;
}

int _full_bytes_needed(int num_bits) { return (num_bits / BYTE_SIZE) + ((num_bits % BYTE_SIZE) != 0); }
