#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "sha256.h"
#include "blockchain.h"

static int transactions_count;

// Get number of chars to hold `num` (NOT including null terminator)
int _num_chars_to_hold_int(int num) {
  // Negative case: first check for UB when trying to take -INT_MIN. The +1 is for the minus sign
  if (num < 0) return _num_chars_to_hold_int((num == INT_MIN) ? INT_MAX : -num) + 1;
  if (num < 10) return 1;
  return 1 + _num_chars_to_hold_int(num / 10);
}

// Get number of chars to (definitely) hold `num` formatted to defined precision (NOT including null terminator)
int _num_chars_to_hold_double(double num) {
  if (num < 0) return _num_chars_to_hold_double(-num) + 1;  // Same as positive + negative sign
  // Integer part + decimal places + decimal point + extra character in case truncation and rounding differ
  return _num_chars_to_hold_int(num) + MAX_AMOUNT_PRECISION + 2;
}

// Get number of chars to (definitely) hold serialisation of `trans` (NOT including null terminator)
int _num_chars_to_hold_transaction_serialisation(transaction trans) {
  return _num_chars_to_hold_int(trans.payee_id) + _num_chars_to_hold_int(trans.payer_id) +
         _num_chars_to_hold_double(trans.amount) + _num_chars_to_hold_int(trans.transaction_id) +
         strlen(" pays   ()");
}

// Get number of chars to (definitely) hold serialisation of `block` (NOT including null terminator)
int _num_chars_to_hold_block_serialisation(block blk) {
  return blk.prev_hash.size + _num_chars_to_hold_transaction_serialisation(blk.trans) + blk.proof_of_work.size +
         strlen("\n\n");
}

// Get a new transaction
transaction transaction_init(double amount, int payer_id, int payee_id) {
  if (amount <= 0) {
    fprintf(stderr, "Cannot initialise transaction with non-positive amount" AMOUNT_FORMAT "\n", amount);
    exit(EXIT_FAILURE);
  }
  return (transaction){amount, payer_id, payee_id, transactions_count++};
}

// Serialise a transaction, `trans` into `buffer`
void transaction_serialise(transaction trans, char *buffer, int buffer_size) {
  int buffer_size_required = _num_chars_to_hold_transaction_serialisation(trans);

  if (buffer_size < buffer_size_required + 1) {
    fprintf(stderr, "Buffer size of %d is not enough to hold serialised transaction (requires size %d).\n",
            buffer_size, buffer_size_required);
    exit(EXIT_FAILURE);
  }

  sprintf(buffer, "%d pays %d " AMOUNT_FORMAT " (%d)", trans.payer_id, trans.payee_id, trans.amount,
          trans.transaction_id);
}

void block_serialise(block blk, char *buffer, char buffer_size) {
  int buffer_size_required = _num_chars_to_hold_block_serialisation(blk) + 1;

  if (buffer_size < buffer_size_required) {
    fprintf(stderr, "Buffer size of %d is not enough to hold serialised block (requires size %d).\n", buffer_size,
            buffer_size_required);
    exit(EXIT_FAILURE);
  }

  char prev_hash_buffer[HASH_SIZE + 1];
  char transaction_buffer[TRANSACTION_SERIALISATION_MAX_CHARS];
  char proof_of_work_buffer[HASH_SIZE + 1];

  bitmap_string_bin(blk.prev_hash, prev_hash_buffer, HASH_SIZE + 1);
  transaction_serialise(blk.trans, transaction_buffer, TRANSACTION_SERIALISATION_MAX_CHARS);
  bitmap_string_bin(blk.proof_of_work, proof_of_work_buffer, HASH_SIZE + 1);

  sprintf(buffer, "%s\n%s\n%s", prev_hash_buffer, transaction_buffer, proof_of_work_buffer);
}
