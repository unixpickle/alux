#ifndef __SCHEDULER_PID_POOL_HPP__
#define __SCHEDULER_PID_POOL_HPP__

#include <utilities/index-set.hpp>
#include <module/module.hpp>

namespace OS {

class Task;

// TODO: make this guy a module!
class PIDPool : public Module {
public:
  static const int IdealMaximum = 0x100;
  static const int MaxExpectedFactor = 0x10;
  
  typedef IndexSet<IdealMaximum, MaxExpectedFactor> IndexSetType;
  
  static void InitGlobal(); // @noncritical
  static PIDPool & GetGlobal(); // @ambicritical
  
  uint64_t AllocPID(Task *); // @noncritical
  void FreePID(uint64_t, Task *); // @noncritical
  
  /**
   * Looks up a task by PID and returns a retained result. If the task could
   * not be found or retained, NULL will be returned.
   * @noncritical
   */
  Task * Find(uint64_t);
  
protected:
  virtual void Initialize();
  virtual DepList GetDependencies();
  
private:
  IndexSetType * indexSet;
  
  uint64_t mapLock OS_ALIGNED(8) = 0;
  Task * buckets[IdealMaximum] = {};
};

}

#endif
