CC=gcc
CFLAGS=-Wall
LDFLAGS=-lm

FOLDER=src
TESTS_OUT=tests.out
TEST_OBJECT=$(FOLDER)/tests.o
OBJECTS=$(FOLDER)/bitmap.o $(FOLDER)/sha256.o $(FOLDER)/blockchain.o

tests: $(TEST_OBJECT) $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TESTS_OUT) $(TEST_OBJECT) $(OBJECTS) $(LDFLAGS)

clean:
	rm -f $(FOLDER)/*.o $(TESTS_OUT)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
