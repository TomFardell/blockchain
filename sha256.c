#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

#define MESSAGE_BLOCK_SIZE 512
#define MESSAGE_LENGTH_SIZE 64

typedef unsigned long long u64;

static bitmap pad_message(const char *message, int char_count) {
  int l = char_count * 8;
  int k = MESSAGE_BLOCK_SIZE - MESSAGE_LENGTH_SIZE - ((l + 1) % MESSAGE_BLOCK_SIZE);
  if (k < 0) k += MESSAGE_BLOCK_SIZE;

  // message | 1 | k zeros | length of message
  int size = l + 1 + k + MESSAGE_LENGTH_SIZE;  // TODO: make the appropriate variables u64s
  bitmap padded_message = bitmap_init(size);

  for (int i = 0; i < char_count; i++) {
    bitmap_set_byte(padded_message, i, message[i]);
  }

  bitmap_set_bit(padded_message, l, 1);

  // Iterate through 00000000000000ff, 000000000000ff00, ..., ff00000000000000 and enumerate in i
  for (u64 window = 255, i = 0; window > 0; window <<= BYTE_SIZE, i++) {
    byte this_byte = (l & window) >> (BYTE_SIZE * i);  // The ith digit of l in base 256
    bitmap_set_byte(padded_message, (padded_message.size / BYTE_SIZE) - i - 1, this_byte);
  }

  return padded_message;
}

int main() {
  bitmap bmap = bitmap_init(20);

  bitmap_set_bit(bmap, 3, 1);
  bitmap_set_bit(bmap, 5, 1);
  bitmap_set_bit(bmap, 9, 1);
  for (int k = 11; k < 20; k++) {
    bitmap_set_bit(bmap, k, 1);
  }
  bitmap_set_bit(bmap, 18, 0);
  bitmap_set_byte(bmap, 1, 'a');

  bitmap_print_bin_on_line(bmap);
  bitmap_print_den_on_line(bmap);
  bitmap_free(bmap);

  bitmap padded_message = pad_message(
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaa",
      120);
  bitmap_print_bin_on_line(padded_message);
  bitmap_free(padded_message);

  return EXIT_SUCCESS;
}
