#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "bitmap.h"

// This is very low (so the program runs quickly)
#define POW_LEADING_ZEROS 6

#define MAX_AMOUNT_PRECISION 6
#define AMOUNT_FORMAT "%.6lf"

#define U64_MAX_CHARS 20
#define TRANSACTION_SERIALISATION_MAX_CHARS 40
#define BLOCK_SERIALISATION_MAX_CHARS 130

// TODO: Add RSA public/private keys here instead of just IDs
typedef struct transaction {
  double amount;
  int payer_id;
  int payee_id;
  int transaction_id;
} transaction;

// For simplicity, a block contains only one transaction
typedef struct block {
  bitmap prev_hash;
  transaction trans;
  u64 proof_of_work;
} block;

typedef struct chain_node {
  block blk;
  struct chain_node *prev;
  int index;  // Index in the chain, i.e. genesis block would have 0 index
} chain_node;

typedef struct chain {
  chain_node *start;
  chain_node *end;
  int size;
} chain;

transaction transaction_init(double amount, int payer_id, int payee_id);
void transaction_serialise(transaction trans, char *buffer, int buffer_size);
void transaction_print_on_line(transaction trans);

block block_init_genesis(transaction trans);
block block_init(block prev_blk, transaction trans);
void block_serialise(block blk, char *buffer, int buffer_size);
bitmap block_hash(block blk);
void block_find_proof_of_work(block *blk);
int block_proof_of_work_is_valid(block blk);
int block_prev_block_hash_matches(block prev_blk, block curr_blk);
void block_free(block *blk);

chain_node *chain_node_init(chain_node *prev_node, transaction trans);
void chain_node_free(chain_node *node);

chain chain_init();
void chain_add_node(chain *chn, transaction trans);
void chain_free(chain *chn);

int _num_chars_to_hold_int(int num);
int _num_chars_to_hold_double(double num);
int _num_chars_to_hold_transaction_serialisation(transaction trans);
int _num_chars_to_hold_block_serialisation(block blk);

#endif
