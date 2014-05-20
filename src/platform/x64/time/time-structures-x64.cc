#include "time-structures-x64.hpp"

namespace OS {
namespace x64 {

uint64_t Time128::GetLower() {
  return lower;
}

uint64_t Time128::GetHigher() {
  return higher;
}

Time128::Time128(uint64_t low, uint64_t high) : lower(low), higher(high) { }

Time128::Time128(const Time128 & time)
  : lower(time.lower), higher(time.higher) { }

Time128 Time128::operator+(const Time128 & time) {
  uint64_t lowSum = time.lower + lower;
  uint64_t highSum = time.higher + higher;
  if (lowSum < lower) {
    highSum++;
  }
  // TODO: see if I can use the constructor directly
  Time128 t(lowSum, highSum);
  return t;
}

Time128 Time128::operator+(uint64_t num) {
  uint64_t lowSum = lower + num;
  uint64_t highSum = higher;
  if (lowSum < lower) {
    highSum++;
  }
  // TODO: see if I can use the constructor directly
  Time128 t(lowSum, highSum);
  return t;
}

Time128 & Time128::operator=(const Time128 & time) {
  lower = time.lower;
  higher = time.higher;
}

}
}

