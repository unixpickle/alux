#ifndef __UTILITIES_CRITICAL_H__
#define __UTILITIES_CRITICAL_H__

#include <platform/interrupts.hpp>

namespace OS {

class ScopeCritical {
private:
  bool wereEnabled;
  
public:
  ScopeCritical();
  ~ScopeCritical();
};

}

#endif
