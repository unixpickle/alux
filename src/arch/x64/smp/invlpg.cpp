#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/vmm/invlpg.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <utilities/critical.hpp>
#include <utilities/lock.hpp>

#include <arch/general/failure.hpp> // TODO: delete this

namespace OS {

namespace x64 {

static bool distributeInvlpg = false;

static void HandleInvlpg();

void DistributeKernelInvlpg(VirtAddr start, size_t size) {
  ScopeCritical critical;
  Invlpg(start, size);
  if (!distributeInvlpg) return;
  
  // create a structure, pass it to the IPI, QED
  CPUList & list = CPUList::GetGlobal();
  CPU & current = list.GetCurrent();
  LAPIC & lapic = LAPIC::GetCurrent();
  for (int i = 0; i < list.GetCount(); i++) {
    CPU & cpu = list[i];
    if (&cpu == &current) continue;
    InvlpgInfo info = {NULL, start, size, false};
    {
      ScopeCriticalLock lock(&cpu.invlpgLock);
      if (cpu.invlpgInfo) {
        info.next = cpu.invlpgInfo;
      }
      cpu.invlpgInfo = &info;
    }
    lapic.SendIPI(cpu.GetAPICID(), IntVectors::Invlpg);
    while (!info.done) { }
  }
}

void InitializeInvlpg() {
  // register the interrupt
  IRT::GetGlobal()[IntVectors::Invlpg] = HandleInvlpg;
  distributeInvlpg = true;
}

static void HandleInvlpg() {
  CPU & cpu = CPUList::GetGlobal().GetCurrent();
  ScopeCriticalLock lock(&cpu.invlpgLock);
  while (cpu.invlpgInfo) {
    InvlpgInfo * nextInfo = cpu.invlpgInfo->next;
    Invlpg(cpu.invlpgInfo->start, cpu.invlpgInfo->size);
    cpu.invlpgInfo->done = true;
    cpu.invlpgInfo = nextInfo;
  }
}

}

}