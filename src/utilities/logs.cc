#include <utilities/logs.h>

namespace OS {

int Log2Ceil(uint64_t num) {
  assert(num);
  // TODO: binary search here!
  // TODO: fix sign comparison warnings
  for (int i = 0; i < 64; i++) {
    if ((1L << i) >= num) return i;
  }
  return -1;
}

int Log2Floor(uint64_t num) {
  assert(num != 0);
  // TODO: binary search here
  for (int i = 0; i < 64; i++) {
    if ((1L << i) > num) {
      return i - 1;
    } else if ((1 << i) == num) {
      return i;
    }
  }
  return -1;
}

}
