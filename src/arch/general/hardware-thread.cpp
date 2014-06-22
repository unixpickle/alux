#include <arch/general/hardware-thread.hpp>
#include <scheduler/general/task.hpp>
#include <critical>
#include <lock>

namespace OS {

void HardwareThread::SetThread(Thread * th) {
  AssertCritical();
  HardwareThread::GetCurrent().SetCurrentThread(th);
}

Thread * HardwareThread::GetThread() {
  AssertCritical();
  return HardwareThread::GetCurrent().GetCurrentThread();
}

void HardwareThread::SetCurrentThread(Thread * th) {
  ScopeCriticalLock scope(&threadLock);
  thread = th;
}

Thread * HardwareThread::GetCurrentThread() {
  ScopeCriticalLock scope(&threadLock);
  return thread;
}

}
