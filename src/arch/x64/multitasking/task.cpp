#include <arch/x64/multitasking/task.hpp>

namespace OS {

namespace x64 {

Task::Task(OS::TaskGroup * g) : super(g) {}

void Task::Run() {
  __asm__ __volatile__(
    "mov %%rax, %%cr3\n"
    "sub $0x28, %%rsp\n"
    "mov $5, %%rcx\n"
    "mov %%rsp, %%rdx\n"
    "rep movsq\n"
    "iretq"
    : : "a" (regs.cr3), "S" (&regs)
  );
}

}

}
