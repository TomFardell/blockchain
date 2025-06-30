#ifndef BITMAP_H
#define BITMAP_H

#define BYTE_SIZE 8

typedef unsigned char byte;

typedef struct bitmap {
  int size;
  byte *map;
} bitmap;

bitmap bitmap_init(const int size);
int bitmap_get_bit(const bitmap bmap, const int index);
void bitmap_set_bit(bitmap bmap, const int index, const int new_value);
void bitmap_set_byte(bitmap bmap, const int byte_index, const byte new_value);

void bitmap_print_bin(const bitmap bmap);
void bitmap_print_bin_on_line(const bitmap bmap);
void bitmap_print_hex(const bitmap bmap);
void bitmap_print_den(const bitmap bmap);
void bitmap_print_den_on_line(const bitmap bmap);

void bitmap_delete(bitmap bmap);

#endif
