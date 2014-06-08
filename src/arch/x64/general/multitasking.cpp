#include <arch/general/multitasking.hpp>
#include <arch/x64/multitasking/kernel-task.hpp>
#include <arch/x64/multitasking/kernel-thread.hpp>
#include <cstddef>

namespace OS {

OS::Task * CreateKernelTask() {
  return new x64::KernelTask();
}

OS::Thread * CreateKernelThread(OS::Task * t, void * function) {
  return CreateKernelThread(t, function, NULL);
}

OS::Thread * CreateKernelThread(OS::Task * t, void * function,
                                void * argument) {
  return new x64::KernelThread(t, (uint64_t)function, (uint64_t)argument);
}

}
