#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#define MAX_AMOUNT_PRECISION 6
#define AMOUNT_FORMAT "%.6lf"

// TODO: Add RSA public/private keys here instead of just IDs
typedef struct transaction {
  double amount;
  int payer_id;
  int payee_id;
  int transaction_id;
} transaction;

transaction transaction_init(double amount, int payer_id, int payee_id);
void transaction_serialise(transaction trans, char *buffer, int buffer_size);

int _num_chars_to_hold_int_as_string(int num);
#endif
