#include "bitmap.h"
#include "sha256.h"

bitmap _pad_message(const char *message, int char_count) {
  int l = char_count * 8;
  int k = MESSAGE_BLOCK_SIZE - MESSAGE_LENGTH_SIZE - ((l + 1) % MESSAGE_BLOCK_SIZE);
  if (k < 0) k += MESSAGE_BLOCK_SIZE;

  // message | 1 | k zeros | length of message
  int size = l + 1 + k + MESSAGE_LENGTH_SIZE;  // TODO: make the appropriate variables u64s
  bitmap padded_message = bitmap_init_zeros(size);

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
