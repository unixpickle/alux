#include <arch/x64/multitasking/thread.hpp>
#include <arch/x64/smp/cpu-list.hpp>
#include <utilities/critical.hpp>

namespace OS {

namespace x64 {

Thread::Thread(OS::Task * g) : super(g) {
  AssertNoncritical();
  kernStack = (void *)(new uint8_t[0x4000]);
}

Thread::~Thread() {
  AssertNoncritical();
  delete (uint8_t *)kernStack;
}

void Thread::Run() {
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

void Thread::SetStackInCPU() {
  AssertCritical();
  CPU & cpu = CPUList::GetGlobal().GetCurrent();
  cpu.tss->rsp[0] = (uint64_t)kernStack + 0x4000; // for interrupts
  cpu.tss->ist[0] = (uint64_t)kernStack + 0x4000; // for NMI
  cpu.tss->ist[1] = (uint64_t)kernStack + 0x4000; // for MCE
}

}

}
