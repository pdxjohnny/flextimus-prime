#include <stdint.h>

/* Returns the integer value of the bit that is set in a variable.
 * For instance if bit 14 is set it will return 14.
 */
int bit_index(uint32_t bit_check) {
  for (unsigned int i = 0; i < 32; ++i) {
    if (bit_check & (1 << i)) {
      return i;
    }
  }
}
