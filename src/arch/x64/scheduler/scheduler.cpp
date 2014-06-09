#include <arch/general/scheduler.hpp>
#include <arch/x64/scheduler/task.hpp>
#include <arch/x64/scheduler/thread.hpp>
#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <arch/general/failure.hpp>
#include <arch/general/clock.hpp>
#include <utilities/frac.hpp>
#include <utilities/critical.hpp>

namespace OS {

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
  // TODO: save the registers by pushing them to the stack, then switch stack
  // and call the destination method
  Panic("SaveAndTick - NYI");
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

}
