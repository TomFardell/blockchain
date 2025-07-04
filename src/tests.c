#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "sha256.h"

#define NUM_BITMAP_TESTS 16
#define NUM_SHA256_TESTS 2

typedef int (*test)(void);

int test_bitmap_1() {
  bitmap bmap1 = bitmap_init_string("00101");
  bitmap bmap2 = bitmap_init_string("00101");
  int result = bitmap_equal(bmap1, bmap2);

  bitmap_free(&bmap1);
  bitmap_free(&bmap2);

  return (result == 1);
}

int test_bitmap_2() {
  bitmap bmap1 = bitmap_init_string("0010000");
  bitmap bmap2 = bitmap_init_string("00100");
  int result = bitmap_equal(bmap1, bmap2);

  bitmap_free(&bmap1);
  bitmap_free(&bmap2);

  return (result == 0);
}

int test_bitmap_3() {
  bitmap bmap1 = bitmap_init_zeros(1);
  bitmap bmap2 = bitmap_init_string("0");
  int result = bitmap_equal(bmap1, bmap2);

  bitmap_free(&bmap1);
  bitmap_free(&bmap2);

  return (result == 1);
}

int test_bitmap_4() {
  bitmap bmap1 = bitmap_init_zeros(9);
  bitmap bmap2 = bitmap_init_zeros(9);
  bitmap_set_bit(&bmap1, 0, 1);
  bitmap_set_bit(&bmap1, 2, 1);
  bitmap_set_byte(&bmap2, 0, 128 + 32);
  int result = bitmap_equal(bmap1, bmap2);

  bitmap_free(&bmap1);
  bitmap_free(&bmap2);

  return (result == 1);
}

int test_bitmap_5() {
  bitmap bmap = bitmap_init_string("1001");
  int result = (bitmap_get_bit(bmap, 0) == 1) + (bitmap_get_bit(bmap, 1) == 0) + (bitmap_get_bit(bmap, 2) == 0) +
               (bitmap_get_bit(bmap, 3) == 1);

  bitmap_free(&bmap);

  return (result == 4);
}

int test_bitmap_6() {
  int result = (_full_bytes_needed(8) == 1) + (_full_bytes_needed(9) == 2) + (_full_bytes_needed(0) == 0) +
               (_full_bytes_needed(15) == 2);
  return (result == 4);
}

int test_bitmap_7() {
  bitmap bmap1 = bitmap_init_string("110111001");
  bitmap bmap2 = bitmap_init_string("101010000");
  bitmap expected = bitmap_init_string("011101001");

  bitmap xor = bitmap_xor(bmap1, bmap2);
  int result = bitmap_equal(expected, xor);

  bitmap_free(&bmap1);
  bitmap_free(&bmap2);
  bitmap_free(&expected);
  bitmap_free(&xor);

  return (result == 1);
}

int test_bitmap_8() {
  bitmap bmap1 = bitmap_init_string("110111001");
  bitmap bmap2 = bitmap_init_string("101010000");
  bitmap expected = bitmap_init_string("111111001");

  bitmap or = bitmap_or(bmap1, bmap2);
  int result = bitmap_equal(expected, or);

  bitmap_free(&bmap1);
  bitmap_free(&bmap2);
  bitmap_free(&expected);
  bitmap_free(&or);

  return (result == 1);
}

int test_bitmap_9() {
  bitmap bmap1 = bitmap_init_string("110111001");
  bitmap bmap2 = bitmap_init_string("101010001");
  bitmap expected = bitmap_init_string("100010001");

  bitmap and = bitmap_and(bmap1, bmap2);
  int result = bitmap_equal(expected, and);

  bitmap_free(&bmap1);
  bitmap_free(&bmap2);
  bitmap_free(&expected);
  bitmap_free(&and);

  return (result == 1);
}

int test_bitmap_10() {
  bitmap bmap = bitmap_init_string("110111001");
  bitmap expected = bitmap_init_string("011100100");

  bitmap lshift = bitmap_lshift(bmap, 2);
  bitmap neg_rshift = bitmap_rshift(bmap, -2);

  int result = bitmap_equal(expected, lshift) + bitmap_equal(expected, neg_rshift);

  bitmap_free(&bmap);
  bitmap_free(&expected);
  bitmap_free(&lshift);
  bitmap_free(&neg_rshift);

  return (result == 2);
}

int test_bitmap_11() {
  bitmap bmap = bitmap_init_string("110111001");
  bitmap expected = bitmap_init_string("001101110");

  bitmap rshift = bitmap_rshift(bmap, 2);
  bitmap neg_lshift = bitmap_lshift(bmap, -2);

  int result = bitmap_equal(expected, rshift) + bitmap_equal(expected, neg_lshift);

  bitmap_free(&bmap);
  bitmap_free(&expected);
  bitmap_free(&rshift);
  bitmap_free(&neg_lshift);

  return (result == 2);
}

int test_bitmap_12() {
  bitmap bmap = bitmap_init_string("110111001");
  bitmap expected = bitmap_init_zeros(9);

  bitmap lshift = bitmap_lshift(bmap, 20);
  bitmap rshift = bitmap_rshift(bmap, 20);

  int result = bitmap_equal(expected, lshift) + bitmap_equal(expected, rshift);

  bitmap_free(&bmap);
  bitmap_free(&expected);
  bitmap_free(&lshift);
  bitmap_free(&rshift);

  return (result == 2);
}

int test_bitmap_13() {
  bitmap bmap = bitmap_init_string("110111001");
  bitmap expected = bitmap_init_string("011100111");

  bitmap lrotate = bitmap_lrotate(bmap, 2);
  bitmap neg_rrotate = bitmap_rrotate(bmap, -2);

  int result = bitmap_equal(expected, lrotate) + bitmap_equal(expected, neg_rrotate);

  bitmap_free(&bmap);
  bitmap_free(&expected);
  bitmap_free(&lrotate);
  bitmap_free(&neg_rrotate);

  return (result == 2);
}

int test_bitmap_14() {
  bitmap bmap = bitmap_init_string("110111001");
  bitmap expected = bitmap_init_string("011101110");

  bitmap rrotate = bitmap_rrotate(bmap, 2);
  bitmap neg_lrotate = bitmap_lrotate(bmap, -2);

  int result = bitmap_equal(expected, rrotate) + bitmap_equal(expected, neg_lrotate);

  bitmap_free(&bmap);
  bitmap_free(&expected);
  bitmap_free(&rrotate);
  bitmap_free(&neg_lrotate);

  return (result == 2);
}

int test_bitmap_15() {
  bitmap bmap = bitmap_init_string("001101001");
  bitmap expected = bitmap_init_string("101");

  bitmap sliced = bitmap_slice(bmap, 3, 6);

  int result = bitmap_equal(expected, sliced);

  bitmap_free(&bmap);
  bitmap_free(&expected);
  bitmap_free(&sliced);

  return (result == 1);
}

int test_bitmap_16() {
  bitmap bmap1 = bitmap_init_number(703, 3);
  bitmap bmap2 = bitmap_init_number(16777919, 3);
  bitmap bmap3 = bitmap_init_zeros(24);
  bitmap expected = bitmap_init_string("000000000000001010111111");

  bitmap_set_bytes_from_number(&bmap3, 2, 1, 1);
  bitmap_set_bytes_from_number(&bmap3, 191, 2, 1);

  int result = bitmap_equal(expected, bmap1) + bitmap_equal(expected, bmap2) + bitmap_equal(expected, bmap3);

  bitmap_free(&bmap1);
  bitmap_free(&bmap2);
  bitmap_free(&bmap3);
  bitmap_free(&expected);

  return (result == 3);
}

int test_sha256_1() {
  bitmap padded = _pad_message(
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aa"
      "aaaaaaaaaaaaa",
      120);
  bitmap expected = bitmap_init_string(
      "011000010110000101100001011000010110000101100001011000010110000101100001011000010110000101100001011000010"
      "11"
      "000010110000101100001011000010110000101100001011000010110000101100001011000010110000101100001011000010110"
      "00"
      "010110000101100001011000010110000101100001011000010110000101100001011000010110000101100001011000010110000"
      "10"
      "110000101100001011000010110000101100001011000010110000101100001011000010110000101100001011000010110000101"
      "10"
      "000101100001011000010110000101100001011000010110000101100001011000010110000101100001011000010110000101100"
      "00"
      "101100001011000010110000101100001011000010110000101100001011000010110000101100001011000010110000101100001"
      "01"
      "100001011000010110000101100001011000010110000101100001011000010110000101100001011000010110000101100001011"
      "00"
      "001011000010110000101100001011000010110000101100001011000010110000101100001011000010110000101100001011000"
      "01"
      "011000010110000101100001011000010110000101100001011000010110000101100001011000010110000101100001011000011"
      "00"
      "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00"
      "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00"
      "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00"
      "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00"
      "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00"
      "00000000000000000000000000001111000000");
  int result = bitmap_equal(padded, expected);

  bitmap_free(&padded);
  bitmap_free(&expected);

  return (result == 1);
}

int test_sha256_2() {
  bitmap padded = _pad_message("abc", 3);
  bitmap expected = bitmap_init_string(
      "011000010110001001100011100000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00"
      "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00"
      "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00"
      "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00"
      "000000000000000000000000000000000000000000000000000000000000000000000000000000011000");
  int result = bitmap_equal(padded, expected);

  bitmap_free(&padded);
  bitmap_free(&expected);

  return (result == 1);
}

int test_bitmap_full() {
  printf("Commencing %d bitmap tests.\n", NUM_BITMAP_TESTS);
  test tests[NUM_BITMAP_TESTS] = {&test_bitmap_1,  &test_bitmap_2,  &test_bitmap_3,  &test_bitmap_4,
                                  &test_bitmap_5,  &test_bitmap_6,  &test_bitmap_7,  &test_bitmap_8,
                                  &test_bitmap_9,  &test_bitmap_10, &test_bitmap_11, &test_bitmap_12,
                                  &test_bitmap_13, &test_bitmap_14, &test_bitmap_15, &test_bitmap_16};
  int passed_tests = 0;

  for (int i = 0; i < NUM_BITMAP_TESTS; i++) {
    if (tests[i]())
      passed_tests++;
    else
      printf("> Test %d failed.\n", i + 1);
  }

  printf("Passed %d/%d bitmap tests.\n", passed_tests, NUM_BITMAP_TESTS);

  return passed_tests;
}

int test_sha256_full() {
  printf("Commencing %d SHA-256 tests.\n", NUM_SHA256_TESTS);
  test tests[NUM_SHA256_TESTS] = {&test_sha256_1, &test_sha256_2};
  int passed_tests = 0;

  for (int i = 0; i < NUM_SHA256_TESTS; i++) {
    if (tests[i]())
      passed_tests++;
    else
      printf("> Test %d failed.\n", i + 1);
  }

  printf("Passed %d/%d SHA-256 tests.\n", passed_tests, NUM_SHA256_TESTS);

  return passed_tests;
}

int main() {
  int passed_tests = 0;
  passed_tests += test_bitmap_full();
  printf("\n");
  passed_tests += test_sha256_full();
  printf("\nPassed %d/%d tests.\n", passed_tests, NUM_BITMAP_TESTS + NUM_SHA256_TESTS);

  return EXIT_SUCCESS;
}
