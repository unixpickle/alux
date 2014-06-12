#include <arch/general/hardware-thread.hpp>
#include <scheduler/general/task.hpp>
#include <utilities/critical.hpp>
#include <utilities/lock.hpp>

namespace OS {

void HardwareThread::SetThread(Thread * th) {
  AssertCritical();
  HardwareThread & hwth = HardwareThread::GetCurrent();
  ScopeCriticalLock scope(&hwth.threadLock);
  if (hwth.thread) {
    hwth.thread->GetTask()->Release();
  }
  hwth.thread = th;
}

Thread * HardwareThread::GetThread() {
  AssertCritical();
  ScopeCriticalLock scope(&threadLock);
  return thread;
}

bool HardwareThread::IsRunningTask(Task * t) {
  AssertCritical();
  ScopeCriticalLock scope(&threadLock);
  return t == thread->GetTask();
}

}
