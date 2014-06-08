#include <multitasking/thread.hpp>
#include <multitasking/task.hpp>
#include <arch/general/cpu.hpp>
#include <utilities/critical.hpp>

namespace OS {

Thread::Thread(Task * _task) : task(_task) {
}

Task * Thread::GetTask() {
  return task;
}

Scheduler::JobGroup * Thread::GetJobGroup() {
  return static_cast<Scheduler::JobGroup *>(task);
}

void Thread::Run() {
  AssertCritical();
  CPU::GetCurrent().SetThread(this);
}

}
