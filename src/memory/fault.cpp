#include <arch/general/hardware-thread.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <scheduler/user/user-task.hpp>
#include <scheduler/user/kill-reasons.hpp>
#include <critical>
#include <cassert>
#include <panic>

namespace OS {

void HandleMemoryFault(VirtAddr addr, bool exec, bool write) {
  AssertCritical();
  
  Thread * th = HardwareThread::GetThread();
  
  assert(th != NULL);
  Task * t = th->GetTask();
  if (t->IsKernel()) {
    Panic("kernel thread page-faulted");
  }
  
  UserTask * task = static_cast<UserTask *>(t);
  bool result = task->GetCodeMap().HandleFault(addr, exec, write);
  if (result) return;
  
  Task::Exit(KillReasons::MemoryFault);
}

}
