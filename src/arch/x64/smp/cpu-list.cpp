#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/smp/init.hpp>
#include <arch/x64/smp/invlpg.hpp>
#include <arch/x64/segments/gdt.hpp>
#include <arch/x64/interrupts/lapic.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <arch/x64/interrupts/errors.hpp>
#include <arch/x64/acpi/acpi-module.hpp>
#include <arch/x64/acpi/madt.hpp>
#include <arch/x64/general/panic.hpp>
#include <arch/general/clock.hpp>
#include <memory/malloc.hpp>
#include <critical>
#include <cassert>
#include <panic>
#include <new>

namespace OS {

HardwareThreadList & HardwareThreadList::GetGlobal() {
  return x64::CPUList::GetGlobal();
}

namespace x64 {

static CPUList globalList;

void CPUList::InitGlobal() {
  new(&globalList) CPUList();
}

CPUList & CPUList::GetGlobal() {
  return globalList;
}

void CPUList::Initialize() {
  AssertNoncritical();
  
  capacity = MADT::GetGlobal()->CountLAPICs(false);
  buffer = new uint8_t[sizeof(CPU) * capacity];
  
  SetCritical(true);
  uint32_t lapicId = LAPIC::GetCurrent().GetId();
  SetCritical(false);
  
  CPU & cpu = AddEntry(lapicId);
  {
    ScopeCritical critical;
    cpu.LoadGS();
    GDT::GetGlobal().Set();
    __asm__ volatile("ltr %%ax" : : "a" (cpu.GetTSSSelector()));
  }
  
  StartProcessors();
  InitializePanic();
  InitializeInvlpg();
}

DepList CPUList::GetDependencies() {
  return DepList(&Malloc::GetGlobal(), &GDT::GetGlobal(),
                 &LAPICModule::GetGlobal(), &IRT::GetGlobal(),
                 &ACPIModule::GetGlobal(), &ClockModule::GetGlobal(),
                 &InterruptErrors::GetGlobal());
}

int CPUList::GetCount() {
  return count;
}

CPU & CPUList::AddEntry(uint32_t apicId) {
  AssertNoncritical();
  assert(count < capacity);
  void * addr = (void *)(buffer + sizeof(CPU) * count);
  new(addr) CPU(apicId);
  return (*this)[count++];
}

CPU & CPUList::operator[](int idx) {
  return *((CPU *)buffer + idx);
}

int CPUList::GetIndex(CPU & entry) {
  uintptr_t ptr = (uintptr_t)&entry;
  uintptr_t diff = ptr - (uintptr_t)buffer;
  return (int)(diff / sizeof(CPU));
}

int CPUList::IndexOf(HardwareThread & th) {
  return CPUList::GetIndex(static_cast<CPU &>(th));
}

}

}
