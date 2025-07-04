#ifndef BITMAP_H
#define BITMAP_H

#define BYTE_SIZE 8
#define BYTE_COMBINATIONS 256

typedef unsigned char byte;
typedef unsigned long long u64;

typedef enum DualOperator { OR, AND, XOR } DualOperator;

typedef struct bitmap {
  int size;
  byte *map;
} bitmap;

bitmap bitmap_init_zeros(int size);
bitmap bitmap_init_string(const char *string);
bitmap bitmap_init_number(u64 number, int bytes);

int bitmap_get_bit(bitmap bmap, int index);
void bitmap_set_bit(bitmap *bmap, int index, int new_value);
void bitmap_set_byte(bitmap *bmap, int byte_index, byte new_value);
void bitmap_set_bytes_from_number(bitmap *bmap, u64 number, int starting_byte, int num_bytes);

int bitmap_equal(bitmap bmap1, bitmap bmap2);
bitmap bitmap_not(bitmap bmap);
bitmap bitmap_or(bitmap bmap1, bitmap bmap2);
bitmap bitmap_and(bitmap bmap1, bitmap bmap2);
bitmap bitmap_xor(bitmap bmap1, bitmap bmap2);
bitmap bitmap_lshift(bitmap bmap, int count);
bitmap bitmap_rshift(bitmap bmap, int count);
bitmap bitmap_lrotate(bitmap bmap, int count);
bitmap bitmap_rrotate(bitmap bmap, int count);
bitmap bitmap_slice(bitmap bmap, int start_index, int end_index);
bitmap bitmap_choose(bitmap choices, bitmap bmap1, bitmap bmap2);

void bitmap_print_bin(bitmap bmap);
void bitmap_print_bin_on_line(bitmap bmap);
void bitmap_print_hex(bitmap bmap);
void bitmap_print_den(bitmap bmap);
void bitmap_print_den_on_line(bitmap bmap);

void bitmap_free(bitmap *bmap);

int _full_bytes_needed(int num_bits);
bitmap _bitmap_dual_operator(bitmap bmap1, bitmap bmap2, DualOperator operation);

#endif
