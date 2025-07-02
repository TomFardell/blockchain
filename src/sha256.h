#include "bitmap.h"

#ifndef SHA256_H
#define SHA256_H

#define MESSAGE_BLOCK_SIZE 512
#define MESSAGE_LENGTH_SIZE 64

typedef unsigned long long u64;

bitmap _pad_message(const char *message, int char_count);

#endif
