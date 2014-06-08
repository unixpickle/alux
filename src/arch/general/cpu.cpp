#include <arch/general/cpu.hpp>
#include <multitasking/task.hpp>
#include <utilities/lock.hpp>
#include <utilities/critical.hpp>

namespace OS {

Thread * CPU::GetThread() {
  AssertCritical();
  ScopeCriticalLock scope(&threadLock);
  return thread;
}

void CPU::SetThread(Thread * t) {
  AssertCritical();
  ScopeCriticalLock scope(&threadLock);
  thread->GetTask()->Release();
  thread = t;
}

}