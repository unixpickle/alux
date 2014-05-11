#include <utilities/logs.h>

namespace OS {

int Log2Ceil(uint64_t num) {
  assert(num);
  // TODO: binary search here!
  for (int i = 0; i < 64; i++) {
    if ((1 << i) >= num) return i;
  }
  return -1;
}

int Log2Floor(uint64_t num) {
  assert(size != 0);
  // TODO: binary search here
  for (int i = 0; i < 64; i++) {
    if ((1 << i) > size) {
      return i - 1;
    } else if ((1 << i) == size) {
      return i;
    }
  }
  return -1;
}

int Log2Ceil(uint32_t num) {
  return Log2Ceil((uint64_t)num);
}

int Log2Floor(uint32_t num) {
  return Log2Floor((uint64_t)num);
}

}