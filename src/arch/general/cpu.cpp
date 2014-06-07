#include <arch/general/cpu.hpp>
#include <multitasking/task-group.hpp>
#include <utilities/lock.hpp>
#include <utilities/critical.hpp>

namespace OS {

Task * CPU::GetTask() {
  AssertCritical();
  ScopeCriticalLock scope(&taskLock);
  return task;
}

void CPU::SetTask(Task * t) {
  AssertCritical();
  ScopeCriticalLock scope(&taskLock);
  task->GetTaskGroup()->Release();
  task = t;
}

}