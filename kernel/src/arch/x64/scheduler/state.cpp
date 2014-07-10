#include <arch/x64/scheduler/state.hpp>
#include <arch/x64/smp/cpu.hpp>
#include <critical>
#include <cstddef>

namespace OS {

State * State::NewKernel(void * call) {
  return new x64::State(call, NULL, true);
}

State * State::NewKernel(void * call, void * arg) {
  return new x64::State(call, arg, true);
}

State * State::NewUser(void * call) {
  return new x64::State(call, NULL, false);
}

State * State::NewUser(void * call, void * arg) {
  return new x64::State(call, arg, false);
}

namespace x64 {

State::State(void * call, void * arg, bool _kernel)
  : rdi((uint64_t)arg), kernel(_kernel) {
  AssertNoncritical();
  
  stack = new uint8_t[0x4000];
  
  if (kernel) {
    state.ss = 0;
    state.rsp = GetStackTop();
    state.cs = 8;
  } else {
    state.ss = 0x1b;
    state.cs = 0x23;
  }
  state.rflags = 0x200;
  state.rip = (uint64_t)call;
}

State::~State() {
  delete stack;
}

void State::Load() {
  AssertCritical();
  if (!kernel) {
    CPU & cpu = CPU::GetCurrent();
    cpu.SetKernelStack((void *)GetStackTop());
  }
  
  if (state.cs & 3) {
    __asm__ __volatile__("swapgs");
  }
  
  __asm__ __volatile__(
    "sub $0x28, %%rsp\n"
    "mov $5, %%rcx\n"
    "mov %%rsp, %%rdi\n"
    "rep movsq\n"
    "mov %%rdx, %%rdi\n"
    "iretq"
    : : "S" (&state), "d" (rdi)
  );
}

void State::Delete() {
  delete this;
}

IRETState * State::GetIRETState() {
  return &state;
}

uint64_t State::GetStackTop() {
  return (uint64_t)stack + 0x4000;
}

}

}
