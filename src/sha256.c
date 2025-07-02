#include "bitmap.h"
#include "sha256.h"

static const int K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

static const int H0[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                          0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

bitmap _pad_message(const char *message, int char_count) {
  int l = char_count * 8;
  int k = MESSAGE_BLOCK_SIZE - MESSAGE_LENGTH_SIZE - ((l + 1) % MESSAGE_BLOCK_SIZE);
  if (k < 0) k += MESSAGE_BLOCK_SIZE;

  // message | 1 | k zeros | length of message
  int size = l + 1 + k + MESSAGE_LENGTH_SIZE;  // TODO: make the appropriate variables u64s
  bitmap padded_message = bitmap_init_zeros(size);

  for (int i = 0; i < char_count; i++) {
    bitmap_set_byte(&padded_message, i, message[i]);
  }

  bitmap_set_bit(&padded_message, l, 1);

  // Iterate through 00000000000000ff, 000000000000ff00, ..., ff00000000000000 and enumerate in i
  for (u64 window = 255, i = 0; window > 0; window <<= BYTE_SIZE, i++) {
    byte this_byte = (l & window) >> (BYTE_SIZE * i);  // The ith digit of l in base 256
    bitmap_set_byte(&padded_message, (padded_message.size / BYTE_SIZE) - i - 1, this_byte);
  }

  return padded_message;
}
