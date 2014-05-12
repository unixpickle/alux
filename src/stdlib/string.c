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

void * memcpy(void * dst, const void * src, size_t n) {
  char * dest = (char *)dst;
  const char * source = (const char *)src;
  while (n--) {
    *(dest++) = *(source++);
  }
  return dst;
}
