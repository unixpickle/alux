#include <math>
#include <cassert>

namespace OS {

const uint64_t ONE = 1;

int Log2Ceil(uint64_t num) {
  assert(num);
  // TODO: binary search here!
  for (int i = 0; i < 64; i++) {
    if ((ONE << i) >= num) return i;
  }
  return -1;
}

int Log2Floor(uint64_t num) {
  assert(num != 0);
  // TODO: binary search here
  for (int i = 0; i < 64; i++) {
    if ((ONE << i) > num) {
      return i - 1;
    } else if ((ONE << i) == num) {
      return i;
    }
  }
  return -1;
}

}
