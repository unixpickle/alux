#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/smp/invlpg.hpp>
#include <arch/x64/vmm/invlpg.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <critical>
#include <lock>
#include <new>

namespace OS {

namespace x64 {

static InvlpgModule gModule;
static void HandleInvlpg();

void InvlpgModule::InitGlobal() {
  new(&gModule) InvlpgModule();
}

InvlpgModule & InvlpgModule::GetGlobal() {
  return gModule;
}

void InvlpgModule::Initialize() {
  IRT::GetGlobal()[IntVectors::Invlpg] = HandleInvlpg;
  isInitialized = true;
}

DepList InvlpgModule::GetDependencies() {
  return DepList(&CPUList::GetGlobal(), &IRT::GetGlobal(),
                 &LAPICModule::GetGlobal());
}

bool InvlpgModule::IsInitialized() {
  return isInitialized;
}

void DistributeKernelInvlpg(VirtAddr start, size_t size) {
  ScopeCritical critical;
  Invlpg(start, size);
  if (!gModule.IsInitialized()) return;
  
  // create a structure, pass it to the IPI, QED
  CPUList & list = CPUList::GetGlobal();
  CPU & current = CPU::GetCurrent();
  LAPIC & lapic = LAPIC::GetCurrent();
  for (int i = 0; i < list.GetCount(); i++) {
    CPU & cpu = list[i];
    if (&cpu == &current) continue;
    InvlpgInfo info = {NULL, start, size, false, true};
    {
      ScopeCriticalLock lock(&cpu.invlpgLock);
      if (cpu.invlpgInfo) {
        info.next = cpu.invlpgInfo;
      }
      cpu.invlpgInfo = &info;
    }
    lapic.SendIPI(cpu.GetId(), IntVectors::Invlpg);
    while (!info.done) { }
  }
}

void DistributeUserInvlpg(VirtAddr start, size_t size, Task * t) {
  ScopeCritical critical;
  Invlpg(start, size, false);
  if (!gModule.IsInitialized()) return;
  
  CPUList & list = CPUList::GetGlobal();
  CPU & current = CPU::GetCurrent();
  LAPIC & lapic = LAPIC::GetCurrent();
  for (int i = 0; i < list.GetCount(); i++) {
    CPU & cpu = list[i];
    if (&cpu == &current) continue;
    if (!cpu.IsRunningTask(t)) continue;
    InvlpgInfo info = {NULL, start, size, false, false};
    {
      ScopeCriticalLock lock(&cpu.invlpgLock);
      if (cpu.invlpgInfo) {
        info.next = cpu.invlpgInfo;
      }
      cpu.invlpgInfo = &info;
    }
    lapic.SendIPI(cpu.GetId(), IntVectors::Invlpg);
    while (!info.done) { }
  }
}

static void HandleInvlpg() {
  AssertCritical();
  LAPIC & lapic = LAPIC::GetCurrent();
  if (lapic.IsInService(IntVectors::Invlpg)) {
    lapic.SendEOI();
  }
  
  CPU & cpu = CPU::GetCurrent();
  ScopeCriticalLock lock(&cpu.invlpgLock);
  while (cpu.invlpgInfo) {
    InvlpgInfo * nextInfo = cpu.invlpgInfo->next;
    Invlpg(cpu.invlpgInfo->start, cpu.invlpgInfo->size,
           cpu.invlpgInfo->isKernel);
    cpu.invlpgInfo->done = true;
    cpu.invlpgInfo = nextInfo;
  }
}

}

}
