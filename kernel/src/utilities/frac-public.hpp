#include <math>

namespace OS {

template <class T>
Frac<T>::Frac(T x, T y) : num(x), denom(y) {
}

template <class T>
T Frac<T>::Multiply(T val) const {
  int logNum = Log2Ceil(num);
  int logVal = Log2Ceil(val);
  int numRemoved = 0;
  T theNum = num;
  T theVal = val;
  while (logNum + logVal >= NumBits) {
    numRemoved++;
    if (logNum > logVal) {
      logNum--;
      theNum >>= 1;
    } else {
      logVal--;
      theVal >>= 1;
    }
  }
  return ((theNum * theVal) / denom) * ((T)1 << numRemoved);
}

}
