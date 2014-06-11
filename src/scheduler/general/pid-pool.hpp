#ifndef __SCHEDULER_PID_POOL_HPP__
#define __SCHEDULER_PID_POOL_HPP__

#include <utilities/index-set.hpp>

namespace OS {

class Task;

class PIDPool {
public:
  static const int IdealMaximum = 0x100;
  static const int MaxExpectedFactor = 0x10;
  
  static void Initialize(); // @noncritical
  static PIDPool & GetGlobal(); // @ambicritical
  
  uint64_t AllocPID(Task *); // @noncritical
  void FreePID(uint64_t, Task *); // @noncritical
  
  /**
   * Looks up a task by PID and returns a retained result. If the task could
   * not be found or retained, NULL will be returned.
   * @noncritical
   */
  Task * Find(uint64_t);
  
private:
  IndexSet<IdealMaximum, MaxExpectedFactor> indexSet;
  
  uint64_t mapLock OS_ALIGNED(8);
  Task * buckets[IdealMaximum];
};

}

#endif
