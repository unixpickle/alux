#include "cstring.hpp"
#include <ansa/cstring>

extern "C" {

void * memcpy(void * dst, const void * src, size_t n) {
  ansa::Memcpy(dst, src, n);
  return dst;
}

void * memset(void * b, int c, size_t len) {
  return ansa::Memset(b, (char)c, len);
}

}
