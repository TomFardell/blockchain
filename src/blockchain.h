#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "bitmap.h"

#define MAX_AMOUNT_PRECISION 6
#define AMOUNT_FORMAT "%.6lf"

#define TRANSACTION_SERIALISATION_MAX_CHARS 40

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
  bitmap proof_of_work;
} block;

transaction transaction_init(double amount, int payer_id, int payee_id);
void transaction_serialise(transaction trans, char *buffer, int buffer_size);

void block_serialise(block blk, char *buffer, char buffer_size);

int _num_chars_to_hold_int(int num);
int _num_chars_to_hold_double(double num);
int _num_chars_to_hold_transaction_serialisation(transaction trans);
int _num_chars_to_hold_block_serialisation(block blk);

#endif
