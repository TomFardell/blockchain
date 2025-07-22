#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blockchain.h"

#define BUFFER_SIZE 20
#define MAX_ID 1023
#define MAX_AMOUNT 10000

void clear_screen() { printf("\e[1;1H\e[2J"); }

void clear_stdin() {
  int c;
  while ((c = fgetc(stdin)) != '\n' && c != EOF);
}

int get_payee_id() {
  char buffer[BUFFER_SIZE];

  while (1) {
    printf("Enter payee ID > ");
    fgets(buffer, BUFFER_SIZE, stdin);

    // Iterate through the buffer to ensure it wasn't overfilled
    int newline_found = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
      if (buffer[i] == '\n') {
        int payee_id;
        int found = sscanf(buffer, "%d", &payee_id);

        if (found && 0 <= payee_id && payee_id <= MAX_ID) return payee_id;

        newline_found = 1;
        break;
      }
    }

    // Only clear stdin if the input was too large, i.e. if no '\n' was found
    if (!newline_found) clear_stdin();

    printf("ID must be a number between 0 and %d.\n", MAX_ID);
  }
}

int get_payer_id() {
  char buffer[BUFFER_SIZE];

  while (1) {
    printf("Enter payer ID > ");
    fgets(buffer, BUFFER_SIZE, stdin);

    int newline_found = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
      if (buffer[i] == '\n') {
        int payer_id;
        int found = sscanf(buffer, "%d", &payer_id);

        if (found && 0 <= payer_id && payer_id <= MAX_ID) return payer_id;

        newline_found = 1;
        break;
      }
    }

    if (!newline_found) clear_stdin();

    printf("ID must be a number between 0 and %d.\n", MAX_ID);
  }
}

double get_amount() {
  char buffer[BUFFER_SIZE];

  while (1) {
    printf("Enter amount > ");
    fgets(buffer, BUFFER_SIZE, stdin);

    int newline_found = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
      if (buffer[i] == '\n') {
        double amount;
        int found = sscanf(buffer, "%lf", &amount);

        if (found && 0 <= amount && amount <= MAX_AMOUNT) return amount;

        newline_found = 1;
        break;
      }
    }

    if (!newline_found) clear_stdin();

    printf("Amount must be a number between 0 and %d.\n", MAX_AMOUNT);
  }
}

void add_transaction(chain *chn) {
  int payee_id = get_payee_id();
  int payer_id = get_payer_id();
  double amount = get_amount();

  printf("\nTransaction of " AMOUNT_FORMAT " from %d to %d added.\nPress ENTER to continue > ", amount, payer_id,
         payee_id);
  clear_stdin();

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

    fgets(buffer, BUFFER_SIZE, stdin);

    // Try to find a newline, and replace with null terminator
    int newline_found = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
      if (buffer[i] == '\n') {
        newline_found = 1;
        buffer[i] = '\0';
        break;
      }
    }

    if (strcmp(buffer, "1") == 0) {
      add_transaction(&chn);
    } else if (strcmp(buffer, "0") == 0) {
      break;
    } else {
      if (!newline_found) clear_stdin();  // If no newline was found then the buffer was exceeded

      printf("Please enter a valid option. Press ENTER to continue > ");
      clear_stdin();
    }
  }

  chain_free(&chn);

  return EXIT_SUCCESS;
}
