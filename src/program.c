#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blockchain.h"

#define BUFFER_SIZE 20
#define MAX_ID 1023

void clear_screen() { printf("\e[1;1H\e[2J"); }

void add_transaction(chain *chn) {
  char buffer[BUFFER_SIZE];

  int payee_id;
  while (1) {
    printf("Enter payee ID > ");
    fgets(buffer, BUFFER_SIZE, stdin);

    int found = sscanf(buffer, "%d", &payee_id);
    if (found && 0 <= payee_id && payee_id <= MAX_ID) break;

    printf("ID must be a number between 0 and %d.\n", MAX_ID);
  }

  int payer_id;
  while (1) {
    printf("Enter payer ID > ");
    fgets(buffer, BUFFER_SIZE, stdin);

    int found = sscanf(buffer, "%d", &payer_id);
    if (found && 0 <= payer_id && payee_id <= MAX_ID) break;

    printf("ID must be a number between 0 and %d.\n", MAX_ID);
  }

  double amount;
  while (1) {
    printf("Enter amount > ");
    fgets(buffer, BUFFER_SIZE, stdin);

    int found = sscanf(buffer, "%lf", &amount);
    if (found && amount >= 0) break;

    printf("Amount must be a positive number.\n");
  }

  printf("Transaction of " AMOUNT_FORMAT " from %d to %d added. Press ENTER to continue > ", amount, payer_id,
         payee_id);
  getchar();

  transaction trans = transaction_init(amount, payer_id, payee_id);
  chain_add_node(chn, trans);
}

int main() {
  char buffer[BUFFER_SIZE];  // Buffer to hold user input
  chain chn = chain_init();

  // TUI loop
  while (1) {
    clear_screen();
    printf("-----| Blockchain Program |-----\n1 - Add transaction\n0 - Quit\nEnter option > ");

    fgets(buffer, BUFFER_SIZE, stdin);  // TODO: Trim whitespace

    if (strcmp("1\n", buffer) == 0)
      add_transaction(&chn);
    else if (strcmp("0\n", buffer) == 0)
      break;
  }

  chain_free(&chn);

  return EXIT_SUCCESS;
}
