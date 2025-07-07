#include <stdio.h>
#include <stdlib.h>
#include "sha256.h"
#include "blockchain.h"

static int transactions_count;

// Get a new transaction
transaction transaction_init(int amount, int payer_id, int payee_id) {
  return (transaction){amount, payer_id, payee_id, transactions_count++};
}
