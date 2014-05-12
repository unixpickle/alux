#include <string.h>
#include <stdint.h>

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

int memcmp(const void * ptr1, const void * ptr2, size_t len) {
  size_t i;
  for (i = 0; i < len; i++) {
    uint8_t b1 = *((uint8_t *)(ptr1 + i));
    uint8_t b2 = *((uint8_t *)(ptr2 + i));
    if (b1 > b2) return 1;
    else if (b1 < b2) return -1;
  }
  return 0;
}
