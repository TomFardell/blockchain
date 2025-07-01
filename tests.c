#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "sha256.h"

#define NUM_BITMAP_TESTS 6
#define NUM_SHA256_TESTS 2

typedef int (*test)(void);

int test_bitmap_1() {
  bitmap bmap1 = bitmap_init_string("00100");
  bitmap bmap2 = bitmap_init_string("00100");
  int result = bitmap_equal(bmap1, bmap2);

  bitmap_free(bmap1);
  bitmap_free(bmap2);

  return (result == 1);
}

int test_bitmap_2() {
  bitmap bmap1 = bitmap_init_string("0010000");
  bitmap bmap2 = bitmap_init_string("00100");
  int result = bitmap_equal(bmap1, bmap2);

  bitmap_free(bmap1);
  bitmap_free(bmap2);

  return (result == 0);
}

int test_bitmap_3() {
  bitmap bmap1 = bitmap_init_zeros(1);
  bitmap bmap2 = bitmap_init_string("0");
  int result = bitmap_equal(bmap1, bmap2);

  bitmap_free(bmap1);
  bitmap_free(bmap2);

  return (result == 1);
}

int test_bitmap_4() {
  bitmap bmap1 = bitmap_init_zeros(9);
  bitmap bmap2 = bitmap_init_zeros(9);
  bitmap_set_bit(bmap1, 0, 1);
  bitmap_set_bit(bmap1, 2, 1);
  bitmap_set_byte(bmap2, 0, 128 + 32);
  int result = bitmap_equal(bmap1, bmap2);

  bitmap_free(bmap1);
  bitmap_free(bmap2);

  return (result == 1);
}

int test_bitmap_5() {
  bitmap bmap = bitmap_init_string("1001");
  int result = (bitmap_get_bit(bmap, 0) == 1) + (bitmap_get_bit(bmap, 1) == 0) + (bitmap_get_bit(bmap, 2) == 0) +
               (bitmap_get_bit(bmap, 3) == 1);

  bitmap_free(bmap);

  return (result == 4);
}

int test_bitmap_6() {
  int result = (_full_bytes_needed(8) == 1) + (_full_bytes_needed(9) == 2) + (_full_bytes_needed(0) == 0) +
               (_full_bytes_needed(15) == 2);
  return (result == 4);
}

int test_sha256_1() {
  bitmap padded = _pad_message(
      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
      "aaaaaaaaaaaaa",
      120);
  bitmap expected = bitmap_init_string(
      "01100001011000010110000101100001011000010110000101100001011000010110000101100001011000010110000101100001011"
      "00001011000010110000101100001011000010110000101100001011000010110000101100001011000010110000101100001011000"
      "01011000010110000101100001011000010110000101100001011000010110000101100001011000010110000101100001011000010"
      "11000010110000101100001011000010110000101100001011000010110000101100001011000010110000101100001011000010110"
      "00010110000101100001011000010110000101100001011000010110000101100001011000010110000101100001011000010110000"
      "10110000101100001011000010110000101100001011000010110000101100001011000010110000101100001011000010110000101"
      "10000101100001011000010110000101100001011000010110000101100001011000010110000101100001011000010110000101100"
      "00101100001011000010110000101100001011000010110000101100001011000010110000101100001011000010110000101100001"
      "01100001011000010110000101100001011000010110000101100001011000010110000101100001011000010110000101100001100"
      "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000001111000000");
  int result = bitmap_equal(padded, expected);

  bitmap_free(padded);
  bitmap_free(expected);

  return (result == 1);
}

int test_sha256_2() {
  bitmap padded = _pad_message("abc", 3);
  bitmap expected = bitmap_init_string(
      "01100001011000100110001110000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000000000011000");
  int result = bitmap_equal(padded, expected);

  bitmap_free(padded);
  bitmap_free(expected);

  return (result == 1);
}

int test_bitmap_full() {
  printf("Commencing %d bitmap tests.\n", NUM_BITMAP_TESTS);
  test tests[NUM_BITMAP_TESTS] = {&test_bitmap_1, &test_bitmap_2, &test_bitmap_3,
                                  &test_bitmap_4, &test_bitmap_5, &test_bitmap_6};
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
