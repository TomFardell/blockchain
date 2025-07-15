#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "sha256.h"
#include "blockchain.h"

#define NUM_BITMAP_TESTS 23
#define NUM_SHA256_TESTS 5
#define NUM_BLOCKCHAIN_TESTS 2

// Function signature for test functions
typedef int (*test)(void);

int test_bitmap_1() {
  bitmap bmap1 = bitmap_init_string("00101");
  bitmap bmap2 = bitmap_init_string("00101");
  bitmap_set_byte(&bmap2, 0, 41);  // Checking that this function doesn't compare trailing bits

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

int test_bitmap_17() {
  bitmap choices = bitmap_init_string("110011001");
  bitmap bmap1 = bitmap_init_string("100100110");
  bitmap bmap2 = bitmap_init_string("001000111");
  bitmap expected = bitmap_init_string("101000110");

  bitmap choose = bitmap_choose(choices, bmap1, bmap2);

  int result = bitmap_equal(expected, choose);

  bitmap_free(&choices);
  bitmap_free(&bmap1);
  bitmap_free(&bmap2);
  bitmap_free(&expected);
  bitmap_free(&choose);

  return (result == 1);
}

int test_bitmap_18() {
  bitmap bmap = bitmap_init_string("11010011101");
  bitmap expected = bitmap_init_string("00101100010");

  bitmap not = bitmap_not(bmap);

  int result = bitmap_equal(expected, not);

  bitmap_free(&bmap);
  bitmap_free(&expected);
  bitmap_free(&not);

  return (result == 1);
}
int test_bitmap_19() {
  bitmap bmap1 = bitmap_init_string("110011001");
  bitmap bmap2 = bitmap_init_string("100100110");
  bitmap bmap3 = bitmap_init_string("001000111");
  bitmap expected = bitmap_init_string("100000111");

  bitmap majority = bitmap_majority(bmap1, bmap2, bmap3);

  int result = bitmap_equal(expected, majority);

  bitmap_free(&bmap1);
  bitmap_free(&bmap2);
  bitmap_free(&expected);
  bitmap_free(&majority);

  return (result == 1);
}

int test_bitmap_20() {
  bitmap bmap1 = bitmap_init_string("10000101000000000110101110011001");
  bitmap bmap2 = bitmap_init_string("00010101111111111010110100110001");
  bitmap expected = bitmap_init_string("10011011000000000001100011001010");

  bitmap added = bitmap_add_mod(bmap1, bmap2);

  int result = bitmap_equal(expected, added);

  return (result == 1);
}

int test_bitmap_21() {
  bitmap bmap1 = bitmap_init_string("1100110011001");
  bitmap expected = bitmap_init_string("1100110011001");

  bitmap copy = bitmap_copy(bmap1);

  int result = bitmap_equal(expected, copy);

  return (result == 1);
}

int test_bitmap_22() {
  bitmap bmap1 = bitmap_init_string("11110000");
  bitmap bmap2 = bitmap_init_string("00001111");
  bitmap expected = bitmap_init_string("11111111");

  bitmap added = bitmap_add_mod(bmap1, bmap2);

  int result = bitmap_equal(expected, added);

  return (result == 1);
}

int test_bitmap_23() {
  bitmap bmap1 = bitmap_init_string("0000000000000000001111");
  bitmap bmap2 = bitmap_init_string("000000000000000000000");
  bitmap bmap3 = bitmap_init_string("0000000000000000000000000000000000000000000000001");
  bitmap bmap4 = bitmap_init_string("10001000000");
  bitmap bmap5 = bitmap_init_zeros(6);

  bitmap_set_byte(&bmap5, 0, 1);  // Put a 1 outside the bitmap but inside the allocated memory

  int result = (bitmap_leading_zeros(bmap1) == 18) + (bitmap_leading_zeros(bmap2) == 21) +
               (bitmap_leading_zeros(bmap3) == 48) + (bitmap_leading_zeros(bmap4) == 0) +
               (bitmap_leading_zeros(bmap5) == 6);

  return (result == 5);
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

  bitmap_free(&padded);
  bitmap_free(&expected);

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

  bitmap_free(&padded);
  bitmap_free(&expected);

  return (result == 1);
}

int test_sha256_3() {
  bitmap bmap = bitmap_init_string("01101100011011110110001101101011");
  bitmap expected = bitmap_init_string("00000011100011111110100110110000");

  bitmap sigma = _lower_sigma_0(bmap);
  int result = bitmap_equal(expected, sigma);

  bitmap_free(&bmap);
  bitmap_free(&expected);
  bitmap_free(&sigma);

  return (result == 1);
}

int test_sha256_4() {
  bitmap bmap1 = sha256("abc");
  bitmap expected1 = bitmap_init_string(
      "10111010011110000001011010111111100011110000000111001111111010100100000101000001010000001101111001011101101"
      "01110001000100010001110110000000000110110000110100011100101100001011101111010100111001011010000010000111111"
      "110110000111110010000000000001010110101101");

  bitmap bmap2 = sha256("I love coding!!");
  bitmap expected2 = bitmap_init_string(
      "01111110111101111010110010001101111101100110110001101000101001101011001101111011111000011000101001100010110"
      "00000111100100010101010010110100101100111011101001001100111000100000100101010101110000100011011000101001100"
      "100001100011100010011101000011001111010001");

  bitmap bmap3 = sha256(
      "bitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitma"
      "pbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitm"
      "apbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbit"
      "mapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbi"
      "tmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapb"
      "itmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmap"
      "bitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitmapbitma"
      "pbitmapbitmapbitmapbitmapbitmapbitmapbitmap");
  bitmap expected3 = bitmap_init_string(
      "10001100000000111011000001110001010001000110011000100111110001000011101000001010101101100000100011110001110"
      "11011111100010000011100100010100111001011000011101110001101010111001101000010100000100010001101101000101100"
      "011111010011001011010111111100010100101001");

  int result = bitmap_equal(expected1, bmap1) + bitmap_equal(expected2, bmap2) + bitmap_equal(expected3, bmap3);

  return (result == 3);
}

int test_sha256_5() {
  bitmap a = bitmap_init_string("10001110000001001110110010111001");
  bitmap b = bitmap_init_string("10000101101000000111101101011111");
  bitmap c = bitmap_init_string("11100101000000110000001110000000");
  bitmap expected = bitmap_init_string("10011011000000000001100011001010");

  bitmap sig0 = _upper_sigma_0(a);
  bitmap maj = bitmap_majority(a, b, c);
  bitmap added = bitmap_add_mod(sig0, maj);

  int result = bitmap_equal(expected, added);

  bitmap_free(&a);
  bitmap_free(&b);
  bitmap_free(&c);
  bitmap_free(&sig0);
  bitmap_free(&maj);
  bitmap_free(&added);
  bitmap_free(&expected);

  return (result == 1);
}

int test_blockchain_1() {
  transaction t1 = transaction_init(100, 0, 1);
  transaction t2 = transaction_init(200, 0, 1);
  transaction t3 = transaction_init(300, 1, 0);

  int result = (t1.transaction_id == 0) + (t2.transaction_id == 1) + (t3.transaction_id == 2);

  return (result == 3);
}

int test_blockchain_2() {
  int n1 = _num_chars_to_hold_int_as_string(20);
  int n2 = _num_chars_to_hold_int_as_string(0);
  int n3 = _num_chars_to_hold_int_as_string(-301);
  int n4 = _num_chars_to_hold_int_as_string(99999);

  int result = (n1 == 3) + (n2 == 2) + (n3 == 5) + (n4 == 6);

  return (result == 4);
}

// Run full bitmap tests
int test_bitmap_full() {
  printf("Commencing %d bitmap tests.\n", NUM_BITMAP_TESTS);
  test tests[NUM_BITMAP_TESTS] = {
      &test_bitmap_1,  &test_bitmap_2,  &test_bitmap_3,  &test_bitmap_4,  &test_bitmap_5,  &test_bitmap_6,
      &test_bitmap_7,  &test_bitmap_8,  &test_bitmap_9,  &test_bitmap_10, &test_bitmap_11, &test_bitmap_12,
      &test_bitmap_13, &test_bitmap_14, &test_bitmap_15, &test_bitmap_16, &test_bitmap_17, &test_bitmap_18,
      &test_bitmap_19, &test_bitmap_20, &test_bitmap_21, &test_bitmap_22, &test_bitmap_23};
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

// Run full SHA-256 tests
int test_sha256_full() {
  printf("Commencing %d SHA-256 tests.\n", NUM_SHA256_TESTS);
  test tests[NUM_SHA256_TESTS] = {&test_sha256_1, &test_sha256_2, &test_sha256_3, &test_sha256_4, &test_sha256_5};
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

// Run blockchain tests
int test_blockchain_full() {
  printf("Commencing %d blockchain tests.\n", NUM_BLOCKCHAIN_TESTS);
  test tests[NUM_BLOCKCHAIN_TESTS] = {test_blockchain_1, test_blockchain_2};
  int passed_tests = 0;

  for (int i = 0; i < NUM_BLOCKCHAIN_TESTS; i++) {
    if (tests[i]())
      passed_tests++;
    else
      printf("> Test %d failed.\n", i + 1);
  }

  printf("Passed %d/%d SHA-256 tests.\n", passed_tests, NUM_BLOCKCHAIN_TESTS);

  return passed_tests;
}

int main() {
  int passed_tests = 0;
  passed_tests += test_bitmap_full();
  printf("\n");
  passed_tests += test_sha256_full();
  printf("\n");
  passed_tests += test_blockchain_full();
  printf("\nPassed %d/%d tests.\n", passed_tests, NUM_BITMAP_TESTS + NUM_SHA256_TESTS + NUM_BLOCKCHAIN_TESTS);

  return EXIT_SUCCESS;
}
