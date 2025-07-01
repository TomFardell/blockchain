#ifndef BITMAP_H
#define BITMAP_H

#define BYTE_SIZE 8

typedef unsigned char byte;

typedef struct bitmap {
  int size;
  byte *map;
} bitmap;

bitmap bitmap_init_zeros(int size);
bitmap bitmap_init_string(const char *string);

int bitmap_get_bit(bitmap bmap, int index);
void bitmap_set_bit(bitmap bmap, int index, int new_value);
void bitmap_set_byte(bitmap bmap, int byte_index, byte new_value);

int bitmap_equal(bitmap bmap1, bitmap bmap2);

void bitmap_print_bin(bitmap bmap);
void bitmap_print_bin_on_line(bitmap bmap);
void bitmap_print_hex(bitmap bmap);
void bitmap_print_den(bitmap bmap);
void bitmap_print_den_on_line(bitmap bmap);

void bitmap_free(bitmap bmap);

int _full_bytes_needed(int num_bits);

#endif
