CC=gcc
CFLAGS=-Wall -Werror

OUT=sha.out
OBJECTS=bitmap.c sha256.c


default:
	$(CC) $(CFLAGS) -o $(OUT) $(OBJECTS) $(LDFLAGS)

clean:
	rm -f *.o $(OUT)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
