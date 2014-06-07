#include <arch/x64/multitasking/task.hpp>
#include <arch/x64/smp/cpu-list.hpp>
#include <utilities/critical.hpp>

namespace OS {

namespace x64 {

Task::Task(OS::TaskGroup * g) : super(g) {
  AssertNoncritical();
  kernStack = (void *)(new uint8_t[0x4000]);
}

Task::~Task() {
  AssertNoncritical();
  delete (uint8_t *)kernStack;
}

void Task::Run() {
  AssertCritical();
  super::Run();
  SetStackInCPU();
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

void Task::SetStackInCPU() {
  AssertCritical();
  CPU & cpu = CPUList::GetGlobal().GetCurrent();
  cpu.tss->rsp[0] = (uint64_t)kernStack + 0x4000; // for interrupts
  cpu.tss->ist[0] = (uint64_t)kernStack + 0x4000; // for NMI
  cpu.tss->ist[1] = (uint64_t)kernStack + 0x4000; // for MCE
}

}

}
