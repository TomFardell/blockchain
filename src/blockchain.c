#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "bitmap.h"
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
  return 2 * _full_bytes_needed(blk.prev_hash.size) + _num_chars_to_hold_transaction_serialisation(blk.trans) +
         U64_MAX_CHARS + strlen("\n\n");
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
            buffer_size, buffer_size_required + 1);
    exit(EXIT_FAILURE);
  }

  sprintf(buffer, "%d pays %d " AMOUNT_FORMAT " (%d)", trans.payer_id, trans.payee_id, trans.amount,
          trans.transaction_id);
}

// Given a transaction, create the genesis block
block block_init_genesis(transaction trans) {
  return (block){bitmap_init_zeros(0), trans, 0};  // Using size 0 bitmap as null
}

// Given a transaction and previous block, create a new block
block block_init(block prev_block, transaction trans) {
  return (block){block_hash(prev_block), trans, 0};  // Initialise with zero POW
}

// Serialise a block, `blk` into `buffer`
void block_serialise(block blk, char *buffer, int buffer_size) {
  int buffer_size_required = _num_chars_to_hold_block_serialisation(blk) + 1;

  if (buffer_size < buffer_size_required) {
    fprintf(stderr, "Buffer size of %d is not enough to hold serialised block (requires size %d).\n", buffer_size,
            buffer_size_required);
    exit(EXIT_FAILURE);
  }

  char prev_hash_buffer[HASH_SIZE_HEX_CHARS + 1];
  char transaction_buffer[TRANSACTION_SERIALISATION_MAX_CHARS];

  bitmap_string_hex(blk.prev_hash, prev_hash_buffer, HASH_SIZE_HEX_CHARS + 1);
  transaction_serialise(blk.trans, transaction_buffer, TRANSACTION_SERIALISATION_MAX_CHARS);

  sprintf(buffer, "%s\n%s\n%llu", prev_hash_buffer, transaction_buffer, blk.proof_of_work);
}

// Get the SHA256 hash of `blk`
bitmap block_hash(block blk) {
  char buffer[BLOCK_SERIALISATION_MAX_CHARS];

  block_serialise(blk, buffer, BLOCK_SERIALISATION_MAX_CHARS);
  return sha256(buffer);
}

// Increment `blk->proof_of_work` until we have at least `POW_LEADING_ZEROS` leading zeros in the block's hash.
// This method should take a while to run
void block_find_proof_of_work(block *blk) {
  // Just keep adding one to the POW until we find one with enough leading zeros
  while (1) {
    bitmap hash = block_hash(*blk);
    int leading_zeros = bitmap_leading_zeros(hash);
    bitmap_free(&hash);

    if (leading_zeros >= POW_LEADING_ZEROS) return;

    blk->proof_of_work++;
  }
}

// Get whether the proof of work stored in `blk` is valid
int block_proof_of_work_is_valid(block blk) {
  bitmap hash = block_hash(blk);
  int result = (bitmap_leading_zeros(hash) >= POW_LEADING_ZEROS);

  bitmap_free(&hash);

  return result;
}

// Get whether the hash of `prev_blk` equals `curr_blk.prev_hash`
int block_prev_block_hash_matches(block prev_blk, block curr_blk) {
  bitmap prev_hash = block_hash(prev_blk);
  int result = bitmap_equal(prev_hash, curr_blk.prev_hash);

  bitmap_free(&prev_hash);

  return result;
}

// Free the memory associated with `blk`
void block_free(block *blk) { bitmap_free(&(blk->prev_hash)); }

// Initialise a chain node on the heap. For this to be a genesis node, have `prev_node` be `NULL`
chain_node *chain_node_init(chain_node *prev_node, transaction trans) {
  chain_node *result = malloc(sizeof *result);  // Allocate the chain node on the heap
  if (!result) {
    fprintf(stderr, "Error allocating memory for chain_node.\n");
    exit(EXIT_FAILURE);
  }

  // Create genesis block if `prev_node` is passed as NULL
  if (prev_node == NULL) {
    result->blk = block_init_genesis(trans);
    result->prev = NULL;
    result->index = 0;
  } else {
    result->blk = block_init(prev_node->blk, trans);
    result->prev = prev_node;
    result->index = prev_node->index + 1;
  }

  return result;
}

// Free the memory associated with `node`, i.e. the stored block and the node itself
void chain_node_free(chain_node *node) {
  block_free(&(node->blk));
  free(node);
  node = NULL;
}

// Initialise a chain of size 0
chain chain_init() { return (chain){NULL, NULL, 0}; }

// Add a new node to the end of the chain, `chn`
void chain_add_node(chain *chn, transaction trans) {
  chain_node *new_node = chain_node_init(chn->end, trans);
  block_find_proof_of_work(&(new_node->blk));
  chn->size++;
  chn->end = new_node;
  if (chn->size == 1) chn->start = new_node;  // If this is the genesis block, also make this node the start
}

// Free the memory associated with the chain, `chn`
void chain_free(chain *chn) {
  chain_node *selected_node = chn->end;
  for (int i = 0; i < chn->size; i++) {
    chain_node *next_selected_node =
        selected_node->prev;  // We are about to free chn->end, so need to get prev here
    chain_node_free(selected_node);
    selected_node = next_selected_node;
  }
}
