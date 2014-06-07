#include <arch/general/multitasking.hpp>
#include <arch/x64/multitasking/kernel-task-group.hpp>
#include <arch/x64/multitasking/kernel-task.hpp>
#include <cstddef>

namespace OS {

TaskGroup * CreateKernelTaskGroup() {
  return new x64::KernelTaskGroup();
}

Task * CreateKernelTask(TaskGroup * g, void * function) {
  return CreateKernelTask(g, function, NULL);
}

Task * CreateKernelTask(TaskGroup * g, void * function, void * argument) {
  return new x64::KernelTask(g, (uint64_t)function, (uint64_t)argument);
}

}
