#include <arch/x64/multitasking/kernel-thread.hpp>
#include <arch/x64/vmm/global-map.hpp>

namespace OS {

namespace x64 {

KernelThread::KernelThread(OS::Task * t, uint64_t method, uint64_t _argument)
  : super(t), argument(_argument) {
  stack = (void *)(new uint8_t[0x4000]);
  
  regs.rip = method;
  regs.cr3 = GlobalMap::GetGlobal().GetPML4();
  regs.ss = 0;
  regs.rsp = (uint64_t)stack + 0x4000;
  __asm__("xor %%rax, %%rax\n"
          "mov %%cs, %%rax"
          : "=a" (regs.cs));
  __asm__("pushfq\n"
          "pop %%rax"
          : "=a" (regs.rflags));
}

KernelThread::~KernelThread() {
  delete (uint8_t *)stack;
}

void KernelThread::Run() {
  if (hasRun) return super::Run();
  
  hasRun = true;
  OS::Thread::Run();
  SetStackInCPU();
  __asm__(
    "mov %%rax, %%cr3\n"
    "sub $0x28, %%rsp\n"
    "mov $5, %%rcx\n"
    "mov %%rsp, %%rdx\n"
    "rep movsq\n"
    "iretq"
    : : "a" (regs.cr3), "S" (&regs), "d" (argument)
  );
}

}

}