#ifndef __SCHEDULER_CODE_MAP_HPP__
#define __SCHEDULER_CODE_MAP_HPP__

#include <scheduler/user/user-code.hpp>
#include <scheduler/general/task.hpp>

namespace OS {

class CodeMap {
public:
  CodeMap(Task *, UserCode *); // @noncritical
  ~CodeMap(); // @noncritical
  
  /**
   * Handle a page fault that was fired for this task. If the page belongs to
   * the code map and the fault was valid, this will return true. Otherwise, it
   * will return false and the fault should be forwarded to some user-space
   * daemon.
   * @noncritical
   */
  bool HandleFault(VirtAddr addr, bool execute, bool write);
  
  VirtAddr GetStart(); // @ambicritical
  
  UserCode * GetUserCode();
  
protected:
  uint64_t lock OS_ALIGNED(8); // @noncritical
  
  struct PageStatus {
    PhysAddr phys; // only if isCopied is true
    bool isCopied;
    bool isMapped;
  };
  PhysAddr allocated;
  
  Task * task;
  UserCode * code;
  
  VirtAddr spaceStart;
  PageStatus * pages;
};

}

#endif
