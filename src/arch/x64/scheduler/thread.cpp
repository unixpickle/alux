#include <arch-specific/thread.hpp>
#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <arch/x64/vmm/user-map.hpp>
#include <scheduler/general/task.hpp>
#include <critical>
#include <panic>

namespace OS {

namespace x64 {

ArchThread::ArchThread(Task * task, bool forKernel) {
  AssertNoncritical();
  kernStack = new uint8_t[0x4000]; // TODO: use slab
  if (forKernel) {
    state.cr3 = GlobalMap::GetGlobal().GetPML4();
    state.ss = 0;
    state.rsp = (uint64_t)kernStack + 0x4000;
    __asm__("xor %%rax, %%rax\n"
            "mov %%cs, %%ax"
            : "=a" (state.cs));
    __asm__("pushfq\n"
            "pop %%rax"
            : "=a" (state.rflags));
  } else {
    UserMap & map = static_cast<UserMap &>(task->GetAddressSpace());
    state.cr3 = map.GetPML4();
    state.ss = 0x23;
    state.cs = 0x1b;
    state.rsp = 0;
    state.rflags = 0x200;
  }
}

ArchThread::~ArchThread() {
  AssertNoncritical();
  delete kernStack; // TODO: use slab
}

void ArchThread::SetKernelCall(void * function, void * argument) {
  state.rip = (uint64_t)function;
  state.rdi = (uint64_t)argument;
}

void ArchThread::SetUserCall(void * function) {
  state.rip = (uint64_t)function;
}

void ArchThread::Run() {
  AssertCritical();
  CPU & cpu = CPU::GetCurrent();
  cpu.GetTSS()->rsp[0] = (uint64_t)kernStack + 0x4000; // for interrupts
  __asm__ __volatile__(
    "mov %%rax, %%cr3\n"
    "sub $0x28, %%rsp\n"
    "mov $5, %%rcx\n"
    "mov %%rsp, %%rdi\n"
    "rep movsq\n"
    "mov %%rdx, %%rdi\n"
    "iretq"
    : : "a" (state.cr3), "S" (&state), "d" (state.rdi)
  );
}

void * ArchThread::GetStackTop() {
  return (void *)(kernStack + 0x4000);
}

}

}
