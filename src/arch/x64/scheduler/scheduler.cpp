#include <arch/general/scheduler.hpp>
#include <arch/x64/scheduler/task.hpp>
#include <arch/x64/scheduler/thread.hpp>
#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <arch/general/failure.hpp>
#include <arch/general/clock.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <utilities/frac.hpp>
#include <utilities/critical.hpp>

namespace OS {

static void CallTick();

Task * CreateKernelTask() {
  return x64::Task::New(true);
}

Thread * CreateKernelThread(OS::Task * kernTask, void * func) {
  return CreateKernelThread(kernTask, func, NULL);
}

Thread * CreateKernelThread(OS::Task * kernTask, void * func, void * arg) {
  x64::Task * theTask = static_cast<x64::Task *>(kernTask);
  x64::Thread * th = x64::Thread::New(theTask, true);
  th->SetEntry((uint64_t)func, (uint64_t)arg);
  return th;
}

void SaveAndTick() {
  AssertCritical();
  x64::CPU & cpu = x64::CPUList::GetGlobal().GetCurrent();
  x64::Thread * th = static_cast<x64::Thread *>(cpu.GetThread());
  if (!th) {
    __asm__("mov %%rax, %%rsp\n"
            "call *%%rbx"
            : : "a" (cpu.dedicatedStack), "b" (CallTick));
  } else {
    __asm__(
      "movq $save_and_tick_return, (%%rdi)\n" // save RIP
      "xor %%rax, %%rax\n"
      "mov %%cs, %%ax\n" // save CS
      "mov %%rax, 8(%%rdi)\n"
      "mov %%ss, %%ax\n" // save SS
      "mov %%rax, 0x20(%%rdi)\n"
      "pushfq\n" // save RFLAGS
      "pop %%rax\n"
      "mov %%rax, 0x10(%%rdi)\n"
      "mov %%rsp, 0x18(%%rdi)\n" // save RSP
      "mov %%rcx, %%rsp\n" // load new RSP
      "call *%%rbx\n" // CallTick()
      "save_and_tick_return:"
      : : "c" (cpu.dedicatedStack), "b" (CallTick), "D" (&th->state)
      : "rax", "rdx", "rsi", "r8", "r9", "r10", "r11", "r12", "r13", "r14",
        "r15", "memory");
  }
}

void SetTimeout(uint64_t deadline, bool) {
  AssertCritical();
  x64::CPU & cpu = x64::CPUList::GetGlobal().GetCurrent();

  uint64_t regularClock = Clock::GetGlobal().GetTicksPerMin();
  Frac64 conversion(cpu.frequencies.lapic, regularClock);
  uint64_t lapicTicks = conversion.Multiply(deadline);

  x64::LAPIC::GetCurrent().SetTimeout(x64::IntVectors::LapicTimer, lapicTicks);
}

void ClearTimeout() {
  AssertCritical();
  x64::LAPIC::GetCurrent().ClearTimeout();
}

void WaitTimeout() {
  AssertCritical();
  __asm__("sti");
  while (1) {
    __asm__ __volatile__("hlt");
  }
}

static void CallTick() {
  Scheduler::GetGlobal().Tick();
}

namespace x64 {

void LapicTickMethod() {
  AssertCritical();
  LAPIC::GetCurrent().SendEOI();
  SaveAndTick();
}

}

}
