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
transaction transaction_init(double amount, int payer_id, int payee_id) {
  if (amount <= 0) {
    fprintf(stderr, "Cannot initialise transaction with non-positive amount" AMOUNT_FORMAT "\n", amount);
    exit(EXIT_FAILURE);
  }
  return (transaction){amount, payer_id, payee_id, transactions_count++};
}

// Serialise a transaction, `trans` into `buffer`
void transaction_serialise(transaction trans, char *buffer, int buffer_size) {
  // Allocate memory for the numbers as strings
  char *str_payee_id = malloc(_num_chars_to_hold_int_as_string(trans.payee_id) * sizeof *str_payee_id);
  char *str_payer_id = malloc(_num_chars_to_hold_int_as_string(trans.payer_id) * sizeof *str_payer_id);
  // Number of chars for integer part + for decimal part + decimal point + 1 extra to account for truncation
  char *str_amount =
      malloc((_num_chars_to_hold_int_as_string(trans.amount) + MAX_AMOUNT_PRECISION + 2) * sizeof *str_amount);
  char *str_transaction_id =
      malloc(_num_chars_to_hold_int_as_string(trans.transaction_id) * sizeof *str_transaction_id);

  if (!str_payee_id || !str_payer_id || !str_amount || !str_transaction_id) {
    fprintf(stderr, "Error allocating memory when serialising transaction.\n");
    exit(EXIT_FAILURE);
  }

  // Convert the numbers to strings
  sprintf(str_payee_id, "%d", trans.payee_id);
  sprintf(str_payer_id, "%d", trans.payer_id);
  sprintf(str_amount, AMOUNT_FORMAT, trans.amount);
  sprintf(str_transaction_id, "%d", trans.transaction_id);

  int buffer_size_required = strlen(str_payee_id) + strlen(str_payer_id) + strlen(str_amount) +
                             strlen(str_transaction_id) + strlen(" pays   ()") + 1;

  if (buffer_size < buffer_size_required) {
    fprintf(stderr, "Buffer size of %d is not enough to hold serialised transaction of length %d.\n", buffer_size,
            buffer_size_required);
    exit(EXIT_FAILURE);
  }

  memset(buffer, 0, buffer_size);
  strcat(buffer, str_payer_id);
  strcat(buffer, " pays ");
  strcat(buffer, str_payee_id);
  strcat(buffer, " ");
  strcat(buffer, str_amount);
  strcat(buffer, " (");
  strcat(buffer, str_transaction_id);
  strcat(buffer, ")");

  free(str_payee_id);
  free(str_payer_id);
  free(str_amount);
  free(str_transaction_id);
}
