#include <arch/general/hardware-thread.hpp>
#include <scheduler/general/task.hpp>
#include <critical>
#include <lock>

#include <panic> // TODO: delete this

namespace OS {

void HardwareThread::SetThread(Thread * th) {
  AssertCritical();
  HardwareThread & hwth = HardwareThread::GetCurrent();
  hwth.SetCurrentThread(th);
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

void HardwareThread::SetCurrentThread(Thread * th) {
  ScopeCriticalLock scope(&threadLock);
  if (thread) {
    thread->GetTask()->Release();
  }
  thread = th;
}

}
