#include <arch/general/hardware-thread.hpp>
#include <scheduler/user/user-task.hpp>
#include <critical>
#include <cassert>
#include <panic>

namespace OS {

void HandleMemoryFault(VirtAddr addr, bool exec, bool write) {
  AssertCritical();
  
  Thread * th = HardwareThread::GetCurrent().GetThread();
  assert(th != NULL);
  Task * t = th->GetTask();
  if (t->IsKernel()) {
    Panic("kernel thread page faulted");
  }
  UserTask * task = static_cast<UserTask *>(t);
  SetCritical(false);
  bool result = task->GetCodeMap().HandleFault(addr, exec, write);
  SetCritical(true);
  
  if (result) return;
  Panic("Unhandled user page fault.");
  // TODO: here, we would ideally kill the task that faulted, but for now I
  // would rather just Panic so we can see what happened.
}

}
