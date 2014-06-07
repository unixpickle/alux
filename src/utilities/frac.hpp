#ifndef __FRAC_HPP__
#define __FRAC_HPP__

namespace OS {

template <class T>
class Frac {
private:
  T num;
  T denom;
  
public:
  static const int NumBits = sizeof(T) * 8;
  
  Frac(T num, T denom);
  
  T Multiply(T num) const;
};

typedef Frac<uint32_t> Frac32;
typedef Frac<uint64_t> Frac64;

}

#include <utilities/frac-public.hpp>

#endif
