#include <arch/x64/scheduler/expert.hpp>
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
#include <new>

namespace OS {

SchedulerExpert & SchedulerExpert::GetGlobal() {
  return x64::SchedulerExpert::GetGlobal();
}

namespace x64 {

static SchedulerExpert globalExpert;

static void CallTick() {
  Scheduler::GetGlobal().Tick();
}

void SchedulerExpert::Initialize() {
  new(&globalExpert) SchedulerExpert();
}

SchedulerExpert & SchedulerExpert::GetGlobal() {
  return globalExpert;
}

OS::Task * SchedulerExpert::CreateKernelTask() {
  return Task::New(true);
}

OS::Thread * SchedulerExpert::CreateKernelThread(OS::Task * kernTask,
                                                 void * func) {
  return CreateKernelThread(kernTask, func, NULL);
}

OS::Thread * SchedulerExpert::CreateKernelThread(OS::Task * kernTask,
                                                 void * func, void * arg) {
  Task * theTask = static_cast<Task *>(kernTask);
  Thread * th = Thread::New(theTask, true);
  th->SetEntry((uint64_t)func, (uint64_t)arg);
  return th;
}

void SchedulerExpert::SaveAndTick() {
  AssertCritical();
  CPU & cpu = CPUList::GetGlobal().GetCurrent();
  Thread * th = static_cast<Thread *>(cpu.GetThread());
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

void SchedulerExpert::SetTimeout(uint64_t deadline, bool) {
  AssertCritical();
  CPU & cpu = CPUList::GetGlobal().GetCurrent();

  uint64_t regularClock = Clock::GetGlobal().GetTicksPerMin();
  Frac64 conversion(cpu.frequencies.lapic, regularClock);
  uint64_t lapicTicks = conversion.Multiply(deadline);

  LAPIC::GetCurrent().SetTimeout(IntVectors::LapicTimer, lapicTicks);
}

void SchedulerExpert::ClearTimeout() {
  AssertCritical();
  LAPIC::GetCurrent().ClearTimeout();
}

void SchedulerExpert::WaitTimeout() {
  AssertCritical();
  __asm__("sti");
  while (1) {
    __asm__ __volatile__("hlt");
  }
}

void LapicTickMethod() {
  AssertCritical();
  LAPIC::GetCurrent().SendEOI();
  SchedulerExpert::GetGlobal().SaveAndTick();
}

}

}
