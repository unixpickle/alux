#ifndef __UTILITIES_CRITICAL_H__
#define __UTILITIES_CRITICAL_H__

#include <platform/interrupts.hpp>
#include <scheduler/scheduler-init.hpp>
#include <cassert>

#define AssertCritical() assert(!HasInitializedScheduler() || \
  !GetInterruptsEnabled())
#define AssertNoncritical() assert(!HasInitializedScheduler() || \
  GetInterruptsEnabled())

namespace OS {

class ScopeCritical {
private:
  bool wereEnabled;
  
public:
  ScopeCritical(); // @ambicritical
  ~ScopeCritical(); // @critical
};

}

#endif
