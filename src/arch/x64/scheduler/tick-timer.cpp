#include <arch/x64/scheduler/tick-timer.hpp>
#include <arch/x64/scheduler/state.hpp>
#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <arch/x64/time/lapic.hpp>
#include <arch/general/clock.hpp>
#include <scheduler-specific/scheduler.hpp>
#include <utilities/frac.hpp>
#include <critical>
#include <iostream>
#include <panic>
#include <new>

namespace OS {

TickTimer & TickTimer::GetGlobal() {
  return x64::TickTimer::GetGlobal();
}

namespace x64 {

static TickTimer globalTimer;

static void CallTick() {
  Scheduler::GetGlobal().Tick();
}

void TickTimer::InitGlobal() {
  new(&globalTimer) TickTimer();
}

TickTimer & TickTimer::GetGlobal() {
  return globalTimer;
}

void TickTimer::Initialize() {
  cout << "Initializing tick timer..." << endl;
  InitializeLapicTimers();
  IRT::GetGlobal()[IntVectors::LapicTimer] = LapicTickMethod;
}

DepList TickTimer::GetDependencies() {
  return DepList(&LAPICModule::GetGlobal(), &ClockModule::GetGlobal(),
                 &CPUList::GetGlobal(), &IRT::GetGlobal(),
                 &OutStreamModule::GetGlobal());
}

void TickTimer::SaveAndTick() {
  AssertCritical();
  CPU & cpu = CPU::GetCurrent();
  Thread * th = cpu.GetThread();
  if (!th) {
    __asm__("mov %%rax, %%rsp\n"
            "call *%%rbx"
            : : "a" (cpu.GetDedicatedStack()), "b" (CallTick));
    __builtin_unreachable();
  } else {
    State & state = static_cast<State &>(th->GetState());
    void * statePtr = (void *)state.GetIRETState();
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
      : : "c" (cpu.GetDedicatedStack()), "b" (CallTick), "D" (statePtr)
      : "rax", "rdx", "rsi", "r8", "r9", "r10", "r11", "r12", "r13", "r14",
        "r15", "memory");
  }
}

void TickTimer::SetTimeout(uint64_t deadline, bool) {
  AssertCritical();
  CPU & cpu = CPU::GetCurrent();

  uint64_t regularClock = Clock::GetClock().GetTicksPerMin();
  Frac64 conversion(cpu.frequencies.lapic, regularClock);
  uint64_t lapicTicks = conversion.Multiply(deadline);

  LAPIC::GetCurrent().SetTimeout(IntVectors::LapicTimer, lapicTicks);
}

void TickTimer::ClearTimeout() {
  AssertCritical();
  LAPIC::GetCurrent().ClearTimeout();
}

void TickTimer::WaitTimeout() {
  AssertCritical();
  __asm__("sti");
  while (1) {
    __asm__ __volatile__("hlt");
  }
  __builtin_unreachable();
}

void TickTimer::TickAtMethod(void (* method)(void *), void * arg) {
  AssertCritical();
  CPU & cpu = CPU::GetCurrent();
  __asm__("mov %%rax, %%rsp\n"
          "call *%%rbx"
          : : "a" (cpu.GetDedicatedStack()), "b" (method), "D" (arg));
  __builtin_unreachable();
}

void LapicTickMethod() {
  AssertCritical();
  LAPIC::GetCurrent().SendEOI();
  TickTimer::GetGlobal().SaveAndTick();
}

}

}
