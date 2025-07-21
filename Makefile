CC=gcc
CFLAGS=-Wall

FOLDER=src

TESTS_OUT=tests.out
TEST_OBJECT=$(FOLDER)/tests.o

PROGRAM_OBJECT=$(FOLDER)/program.o
PROGRAM_OUT=program.out

OBJECTS=$(FOLDER)/bitmap.o $(FOLDER)/sha256.o $(FOLDER)/blockchain.o

program: $(PROGRAM_OBJECT) $(OBJECTS)
	$(CC) $(CFLAGS) $(EXTRAFLAGS) -o $(PROGRAM_OUT) $(PROGRAM_OBJECT) $(OBJECTS) $(LDFLAGS)

tests: $(TEST_OBJECT) $(OBJECTS)
	$(CC) $(CFLAGS) $(EXTRAFLAGS) -o $(TESTS_OUT) $(TEST_OBJECT) $(OBJECTS) $(LDFLAGS)

clean:
	rm -f $(FOLDER)/*.o $(TESTS_OUT) $(PROGRAM_OUT)

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRAFLAGS) -c -o $@ $<
