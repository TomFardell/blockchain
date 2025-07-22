# Blockchain

Simple blockchain implementation by Tom Fardell.

- Blockchain implementation: [blockchain.c](./src/blockchain.c)
- SHA-256 hashing algorithm: [sha256.c](./src/sha256.c)
- Custom bitmap class: [bitmap.c](./src/bitmap.c)

## Program

The main program is pretty barebones and doesn't showcase the SHA-256 hashing.

Build and run the program:
```console
$ make program
$ ./program.out
```

## Tests

Build and run the tests:
```console
$ make tests
$ ./tests.out
```

## What next?

This project does not currently have any wallet implementation. For example, one would use RSA to allow users to digitally sign transactions using a private key.
