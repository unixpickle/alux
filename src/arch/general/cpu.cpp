#include <arch/general/cpu.hpp>
#include <scheduler/base/task.hpp>
#include <utilities/critical.hpp>
#include <utilities/lock.hpp>

namespace OS {

void CPU::SetThread(Thread * th) {
  AssertCritical();
  CPU & cpu = CPU::GetCurrent();
  ScopeCriticalLock scope(&cpu.threadLock);
  if (cpu.thread) {
    cpu.thread->GetTask()->Release();
  }
  cpu.thread = th;
}

Thread * CPU::GetThread() {
  AssertCritical();
  ScopeCriticalLock scope(&threadLock);
  return thread;
}

}
