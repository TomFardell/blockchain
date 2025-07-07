#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

// TODO: Add RSA public/private keys here instead of just IDs
typedef struct transaction {
  int amount;
  int payer_id;
  int payee_id;
  int transaction_id;
} transaction;

transaction transaction_init(int amount, int payer_id, int payee_id);
#endif
