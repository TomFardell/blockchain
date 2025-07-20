#include <string.h>
#include "bitmap.h"
#include "sha256.h"

static const int K0[NUM_WORK_ITERATIONS] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

static const int H0[NUM_WORKING_VARS] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                                         0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

// Convert `message` to a bitmap whose length is a multiple of 512 and is of the correct form for SHA-256
bitmap _pad_message(const char *message, int char_count) {
  int l = char_count * BYTE_SIZE;
  int k = MESSAGE_BLOCK_SIZE - MESSAGE_LENGTH_SIZE - ((l + 1) % MESSAGE_BLOCK_SIZE);
  if (k < 0) k += MESSAGE_BLOCK_SIZE;

  // message | 1 | k zeros | length of message
  int size = l + 1 + k + MESSAGE_LENGTH_SIZE;
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

// SHA-256 helper function
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

// SHA-256 helper function
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

// SHA-256 helper function
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

// SHA-256 helper function
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

// Perform the SHA-256 hashing algorithm on the string `message`, returning the result as a bitmap
bitmap sha256(const char *message) {
  bitmap padded_message = _pad_message(message, strlen(message));

  // Set up K as bitmaps from the constants
  bitmap K[NUM_WORK_ITERATIONS];
  for (int i = 0; i < NUM_WORK_ITERATIONS; i++) {
    K[i] = bitmap_init_number(K0[i], WORD_LENGTH / BYTE_SIZE);
  }

  // Initialise H as bitmaps from the constants
  bitmap H[NUM_WORKING_VARS];
  for (int i = 0; i < NUM_WORKING_VARS; i++) {
    H[i] = bitmap_init_number(H0[i], WORD_LENGTH / BYTE_SIZE);
  }

  int N = padded_message.size / MESSAGE_BLOCK_SIZE;

  for (int i = 0; i < N; i++) {
    bitmap W[SCHEDULE_LENGTH];

    // The first 16 entries of W come from the message block
    for (int t = 0; t < MESSAGE_BLOCK_SIZE / WORD_LENGTH; t++) {
      W[t] = bitmap_slice(padded_message, i * MESSAGE_BLOCK_SIZE + t * WORD_LENGTH,
                          i * MESSAGE_BLOCK_SIZE + (t + 1) * WORD_LENGTH);
    }

    // The remaining entries are defined from a recurrence relation
    for (int t = MESSAGE_BLOCK_SIZE / WORD_LENGTH; t < SCHEDULE_LENGTH; t++) {
      bitmap term1 = _lower_sigma_1(W[t - 2]);
      bitmap term2 = _lower_sigma_0(W[t - 15]);
      bitmap term3 = bitmap_add_mod(term1, W[t - 7]);
      bitmap term4 = bitmap_add_mod(term2, W[t - 16]);
      W[t] = bitmap_add_mod(term3, term4);

      bitmap_free(&term1);
      bitmap_free(&term2);
      bitmap_free(&term3);
      bitmap_free(&term4);
    }

    // a0 b1 c2 d3 e4 f5 g6 h7
    bitmap working_vars[NUM_WORKING_VARS];
    for (int k = 0; k < NUM_WORKING_VARS; k++) {
      working_vars[k] = bitmap_copy(H[k]);
    }

    for (int t = 0; t < NUM_WORK_ITERATIONS; t++) {
      bitmap term1 = _upper_sigma_1(working_vars[4]);
      bitmap term2 = bitmap_choose(working_vars[4], working_vars[5], working_vars[6]);
      bitmap term3 = bitmap_add_mod(working_vars[7], term1);
      bitmap term4 = bitmap_add_mod(term3, term2);
      bitmap term5 = bitmap_add_mod(term4, K[t]);
      bitmap T1 = bitmap_add_mod(term5, W[t]);

      bitmap term6 = _upper_sigma_0(working_vars[0]);
      bitmap term7 = bitmap_majority(working_vars[0], working_vars[1], working_vars[2]);
      bitmap T2 = bitmap_add_mod(term6, term7);

      bitmap_free(&term1);
      bitmap_free(&term2);
      bitmap_free(&term3);
      bitmap_free(&term4);
      bitmap_free(&term5);
      bitmap_free(&term6);
      bitmap_free(&term7);

      // Need to free old bitmaps before replacing in each of the working variables
      bitmap_free(working_vars + 7);
      working_vars[7] = bitmap_copy(working_vars[6]);
      bitmap_free(working_vars + 6);
      working_vars[6] = bitmap_copy(working_vars[5]);
      bitmap_free(working_vars + 5);
      working_vars[5] = bitmap_copy(working_vars[4]);
      bitmap_free(working_vars + 4);
      working_vars[4] = bitmap_add_mod(working_vars[3], T1);
      bitmap_free(working_vars + 3);
      working_vars[3] = bitmap_copy(working_vars[2]);
      bitmap_free(working_vars + 2);
      working_vars[2] = bitmap_copy(working_vars[1]);
      bitmap_free(working_vars + 1);
      working_vars[1] = bitmap_copy(working_vars[0]);
      bitmap_free(working_vars + 0);
      working_vars[0] = bitmap_add_mod(T1, T2);

      bitmap_free(&T1);
      bitmap_free(&T2);
    }

    for (int k = 0; k < NUM_WORKING_VARS; k++) {
      bitmap sum = bitmap_add_mod(working_vars[k], H[k]);
      bitmap_free(H + k);
      bitmap_free(working_vars + k);
      H[k] = sum;
    }

    for (int k = 0; k < SCHEDULE_LENGTH; k++) {
      bitmap_free(W + k);
    }
  }

  bitmap result = bitmap_init_zeros(WORD_LENGTH * NUM_WORKING_VARS);

  for (int i = 0; i < WORD_LENGTH * NUM_WORKING_VARS / BYTE_SIZE; i++) {
    bitmap_set_byte(&result, i, H[(i * BYTE_SIZE) / WORD_LENGTH].map[i % (WORD_LENGTH / BYTE_SIZE)]);
  }

  for (int i = 0; i < NUM_WORK_ITERATIONS; i++) {
    bitmap_free(K + i);
  }

  for (int i = 0; i < NUM_WORKING_VARS; i++) {
    bitmap_free(H + i);
  }
  bitmap_free(&padded_message);

  return result;
}
