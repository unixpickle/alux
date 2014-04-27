#include <string.h>

void * memset(void * ptr, int value, size_t number) {
  unsigned char * dest = (unsigned char *)ptr;
  unsigned char val = value;
  while (number--) {
    *(dest++) = val;
  }
  return ptr;
}
