#ifndef __UTILITIES_CRITICAL_HPP__
#define __UTILITIES_CRITICAL_HPP__

#include <arch/general/critical.hpp>
#include <cassert>

#define AssertCritical() assert(IgnoreCriticality() || \
  GetCritical())
#define AssertNoncritical() assert(IgnoreCriticality() || \
  !GetCritical())

namespace OS {

class ScopeCritical {
private:
  bool wasCritical;

public:
  ScopeCritical(); // @ambicritical (x)
  ~ScopeCritical(); // @critical -> @ambicritical (x)
};

}

#endif
