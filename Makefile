CC=gcc
CFLAGS=-Wall -Werror

OUT=bmap
FILE=bitmap.c

default:
	$(CC) $(CFLAGS) -o $(OUT) $(FILE) $(LDFLAGS)
clean:
	rm $(OUT)
