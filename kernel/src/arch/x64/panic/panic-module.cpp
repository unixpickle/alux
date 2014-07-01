#include <arch/x64/panic/panic-module.hpp>
#include <arch/x64/panic/panic.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <arch/x64/smp/cpu-list.hpp>
#include <new>

namespace OS {

namespace x64 {

static PanicModule gPanic;

void PanicModule::InitGlobal() {
  new(&gPanic) PanicModule();
}

PanicModule & PanicModule::GetGlobal() {
  return gPanic;
}

void PanicModule::Initialize() {
  IRT::GetGlobal()[IntVectors::Panic] = PanicIPI;
  hasBeenInitialized = true;
}

DepList PanicModule::GetDependencies() {
  return DepList(&IRT::GetGlobal(), &CPUList::GetGlobal(),
                 &LAPICModule::GetGlobal());
}

void PanicModule::DistributeError() {
  if (__sync_fetch_and_or(&hasPanicked, 1)) {
    __asm__("hlt");
  }
  
  if (!hasBeenInitialized) return;
  
  // loop through CPUs
  CPUList & list = CPUList::GetGlobal();
  LAPIC & lapic = LAPIC::GetCurrent();
  for (int i = 0; i < list.GetCount(); i++) {
    CPU & cpu = list[i];
    if (cpu.GetId() == lapic.GetId()) continue;
    lapic.SendIPI(cpu.GetId(), IntVectors::Panic);
  }
}

}

}
