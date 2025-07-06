#include "bitmap.h"

#ifndef SHA256_H
#define SHA256_H

#define MESSAGE_BLOCK_SIZE 512
#define MESSAGE_LENGTH_SIZE 64
#define WORD_LENGTH 32
#define SCHEDULE_LENGTH 64
#define NUM_WORKING_VARS 8
#define NUM_WORK_ITERATIONS 64

bitmap sha256(const char *message);

bitmap _pad_message(const char *message, int char_count);
bitmap _lower_sigma_0(bitmap bmap);
bitmap _lower_sigma_1(bitmap bmap);
bitmap _upper_sigma_0(bitmap bmap);
bitmap _upper_sigma_1(bitmap bmap);

#endif
