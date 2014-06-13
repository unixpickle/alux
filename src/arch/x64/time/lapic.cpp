#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <arch/x64/smp/cpu-list.hpp>
#include <arch/general/clock.hpp>
#include <critical>
#include <atomic>
#include <new>

#include <iostream> // TODO: delete this

namespace OS {

namespace x64 {

static Atomic<int> remaining(0);

static void CalibrateLAPIC();

void InitializeLapicTimers() {
  IRT::GetGlobal()[IntVectors::Calibrate] = CalibrateLAPIC;

  new(&remaining) Atomic<int>(CPUList::GetGlobal().GetCount());
  
  SetCritical(true);
  LAPIC & lapic = LAPIC::GetCurrent();
  for (int i = 0; i < CPUList::GetGlobal().GetCount(); i++) {
    CPU & cpu = CPUList::GetGlobal()[i];
    if (cpu.GetId() == lapic.GetId()) continue;
    lapic.SendIPI(cpu.GetId(), IntVectors::Calibrate);
  }
  CalibrateLAPIC();
  SetCritical(false);

  while (remaining.GetValue());
  IRT::GetGlobal().Unset(IntVectors::Calibrate);
}

static void CalibrateLAPIC() {
  AssertCritical();
  
  LAPIC & lapic = LAPIC::GetCurrent();
  if (lapic.IsInService(IntVectors::Calibrate)) {
    lapic.SendEOI();
  }
  lapic.WriteReg(LAPIC::RegLVTTimer, 0xff);
  lapic.WriteReg(LAPIC::RegTimerInitCount, 0xffffffff);
  SetCritical(false);
  Clock::GetGlobal().MicroSleep(500000);
  SetCritical(true);

  uint64_t value = lapic.ReadReg(LAPIC::RegTimerCurrCount);
  lapic.WriteReg(LAPIC::RegLVTTimer, 0x10000);

  CPU & cpu = CPU::GetCurrent();
  cpu.frequencies.lapic = (uint64_t)(0xffffffff - value) * 120;
  
  --remaining;
}

}

}
