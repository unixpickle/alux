#include <arch/x64/multitasking/kernel-task.hpp>

namespace OS {

namespace x64 {

KernelTask::KernelTask(OS::TaskGroup * g, uint64_t method, uint64_t argument)
  : super(g) {
  stack = (void *)(new uint8_t[0x4000]);
}

KernelTask::~KernelTask() {
  delete (uint8_t *)stack;
}

void KernelTask::Run() {
  // TODO: here, run the thing either by doing super::Run() or by setting up
  // all the state for the first time.
}

}

}