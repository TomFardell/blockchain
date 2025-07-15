#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sha256.h"
#include "blockchain.h"

static int transactions_count;

// Get the size of char array required to hold `num` (including null terminator)
int _num_chars_to_hold_int_as_string(int num) {
  if (num == 0) return 2;
  if (num < 0) return _num_chars_to_hold_int_as_string(-num) + 1;  // Same as positive + negative sign
  return (int)(ceil(log10(num))) + 1;                              // Number of digits + null terminator character
}

// Get a new transaction
transaction transaction_init(int amount, int payer_id, int payee_id) {
  return (transaction){amount, payer_id, payee_id, transactions_count++};
}
