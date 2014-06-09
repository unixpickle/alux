#include <arch/x64/scheduler/thread.hpp>
#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <utilities/critical.hpp>

namespace OS {

namespace x64 {

Thread * Thread::New(Task * task, bool forKernel) {
  // TODO: use some sort of slab here
  return new Thread(task, forKernel);
}

Thread::Thread(Task * aTask, bool forKernel) : super(aTask) {
  AssertNoncritical();
  kernStack = new uint8_t[0x4000];
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
  }
}

Thread::~Thread() {
  AssertNoncritical();
  delete kernStack;
}

void Thread::Delete() {
  // TODO: use some sort of slab here
  delete this;
}

void Thread::SetEntry(uint64_t rip, uint64_t rdi) {
  state.rip = rip;
  state.rdi = rdi;
}

void Thread::Run() {
  AssertCritical();
  CPU & cpu = CPUList::GetGlobal().GetCurrent();
  cpu.tss->rsp[0] = (uint64_t)kernStack + 0x4000; // for interrupts
  __asm__ __volatile__(
    "mov %%rax, %%cr3\n"
    "sub $0x28, %%rsp\n"
    "mov $5, %%rcx\n"
    "mov %%rsp, %%rdx\n"
    "rep movsq\n"
    "iretq"
    : : "a" (state.cr3), "S" (&state), "D" (state.rdi)
  );
}

}

}