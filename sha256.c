#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

int main() {
  bitmap bmap = bitmap_init(20);

  bitmap_set_bit(bmap, 3, 1);
  bitmap_set_bit(bmap, 5, 1);
  bitmap_set_bit(bmap, 9, 1);
  for (int k = 11; k < 20; k++) {
    bitmap_set_bit(bmap, k, 1);
  }
  bitmap_set_bit(bmap, 18, 0);
  bitmap_set_byte(bmap, 1, 'a');

  bitmap_print_bin_on_line(bmap);
  bitmap_print_den_on_line(bmap);
  bitmap_delete(bmap);

  return EXIT_SUCCESS;
}
