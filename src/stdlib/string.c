#include <string.h>

void * memset(void * ptr, int value, size_t number) {
  unsigned char * dest = (unsigned char *)ptr;
  unsigned char val = value;
  while (number--) {
    *(dest++) = val;
  }
  return ptr;
}

void bzero(void * ptr, size_t size) {
  memset(ptr, 0, size);
}

