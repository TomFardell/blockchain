#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

  bitmap_set_bytes_from_number(&padded_message, l,
                               (padded_message.size / BYTE_SIZE) - (MESSAGE_LENGTH_SIZE / BYTE_SIZE),
                               MESSAGE_LENGTH_SIZE / BYTE_SIZE);

  return padded_message;
}

bitmap _lower_sigma_0(bitmap bmap) {
  bitmap bmap1 = bitmap_rrotate(bmap, 7);
  bitmap bmap2 = bitmap_rrotate(bmap, 18);
  bitmap bmap3 = bitmap_rshift(bmap, 3);
  bitmap bmap4 = bitmap_xor(bmap1, bmap2);
  bitmap result = bitmap_xor(bmap4, bmap3);

  bitmap_free(&bmap1);
  bitmap_free(&bmap2);
  bitmap_free(&bmap3);
  bitmap_free(&bmap4);

  return result;
}

bitmap _lower_sigma_1(bitmap bmap) {
  bitmap bmap1 = bitmap_rrotate(bmap, 17);
  bitmap bmap2 = bitmap_rrotate(bmap, 19);
  bitmap bmap3 = bitmap_rshift(bmap, 10);
  bitmap bmap4 = bitmap_xor(bmap1, bmap2);
  bitmap result = bitmap_xor(bmap4, bmap3);

  bitmap_free(&bmap1);
  bitmap_free(&bmap2);
  bitmap_free(&bmap3);
  bitmap_free(&bmap4);

  return result;
}

bitmap _upper_sigma_0(bitmap bmap) {
  bitmap bmap1 = bitmap_rrotate(bmap, 2);
  bitmap bmap2 = bitmap_rrotate(bmap, 13);
  bitmap bmap3 = bitmap_rrotate(bmap, 22);
  bitmap bmap4 = bitmap_xor(bmap1, bmap2);
  bitmap result = bitmap_xor(bmap4, bmap3);

  bitmap_free(&bmap1);
  bitmap_free(&bmap2);
  bitmap_free(&bmap3);
  bitmap_free(&bmap4);

  return result;
}

bitmap _upper_sigma_1(bitmap bmap) {
  bitmap bmap1 = bitmap_rrotate(bmap, 6);
  bitmap bmap2 = bitmap_rrotate(bmap, 11);
  bitmap bmap3 = bitmap_rrotate(bmap, 25);
  bitmap bmap4 = bitmap_xor(bmap1, bmap2);
  bitmap result = bitmap_xor(bmap4, bmap3);

  bitmap_free(&bmap1);
  bitmap_free(&bmap2);
  bitmap_free(&bmap3);
  bitmap_free(&bmap4);

  return result;
}

bitmap sha256(const char *message) {
  bitmap padded_message = _pad_message(message, strlen(message));

  // Split up the padded message into 32 bit words
  int num_words = padded_message.size / WORD_LENGTH;
  bitmap *M = malloc(num_words * sizeof *M);
  for (int i = 0; i < num_words; i++) {
    *M = bitmap_slice(padded_message, i * WORD_LENGTH, (i + 1) * WORD_LENGTH);
  }

  return (bitmap){};
}
