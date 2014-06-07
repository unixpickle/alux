#include <arch/x64/multitasking/task.hpp>

namespace OS {

namespace x64 {

Task::Task(OS::TaskGroup * g) : super(g) {}

void Task::Run() {
  // TODO: do a context switch into the given task using an iret and a bunch of
  // inline assembly
}

}

}
