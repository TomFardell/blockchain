CC=gcc
CFLAGS=-Wall -Werror

TESTS_OUT=tests.out
TEST_OBJECT=tests.o
OBJECTS=bitmap.o sha256.o

tests: $(TEST_OBJECT) $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TESTS_OUT) $(TEST_OBJECT) $(OBJECTS) $(LDFLAGS)

clean:
	rm -f *.o $(TESTS_OUT)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
