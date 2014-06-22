#include <arch/x64/vmm/tlb.hpp>
#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <arch/x64/interrupts/vectors.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <critical>
#include <lock>
#include <new>

namespace OS {

namespace x64 {

static TLB gTLB;

static void HandleInvlpg();
static void Invlpg(VirtAddr start, size_t size, bool kernel = true);

void TLB::InitGlobal() {
  new(&gTLB) TLB();
}

TLB & TLB::GetGlobal() {
  return gTLB;
}

void TLB::WillSetUserMap(UserMap * map) {
  if (gTLB.IsUninitialized()) return;
  CPU::GetCurrent().SetCurrentMap(map);
}

void TLB::InvlpgGlobal(VirtAddr start, size_t size) {
  ScopeCritical critical;
  Invlpg(start, size);
  if (gTLB.IsUninitialized()) return;
  
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

void TLB::InvlpgUser(VirtAddr start, size_t size, UserMap * sender) {
  ScopeCritical critical;
  if (gTLB.IsUninitialized()) {
    Invlpg(start, size, false);
    return;
  }
  
  CPU & current = CPU::GetCurrent();
  CPUList & list = CPUList::GetGlobal();
  
  // invalidate the cache on this CPU if we are using the sending space.
  if (current.GetCurrentMap() == sender) {
    Invlpg(start, size, false);
  }
  
  LAPIC & lapic = LAPIC::GetCurrent();
  for (int i = 0; i < list.GetCount(); i++) {
    CPU & cpu = list[i];
    if (&cpu == &current) continue;
    if (cpu.GetCurrentMap() != sender) continue;
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

void TLB::Initialize() {
  IRT::GetGlobal()[IntVectors::Invlpg] = HandleInvlpg;
}

DepList TLB::GetDependencies() {
  return DepList(&CPUList::GetGlobal(), &IRT::GetGlobal(),
                 &LAPICModule::GetGlobal());
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

static void Invlpg(VirtAddr start, size_t size, bool kernel) {
  AssertCritical();
  
  if (size > 0x200000L) {
    // at this point, it's more efficient to just clear all the caches
    if (kernel) {
      __asm__("mov %%cr4, %%rax\n"
              "xor $0x80, %%rax\n"
              "mov %%rax, %%cr4\n"
              "or $0x80, %%rax\n"
              "mov %%rax, %%cr4" : : : "rax", "memory");
    } else {
      __asm__("mov %%cr3, %%rax\n"
              "mov %%rax, %%cr3"
              : : : "rax", "memory");
    }
    return;
  }
  
  // invalidate one cache entry at a time
  for (VirtAddr addr = start; addr < start + size; addr += 0x1000) {
    __asm__("invlpg (%0)" : : "r" (addr) : "memory");
  }
}

}

}
