#include <arch/x64/pmm/region-list.hpp>
#include <arch/x64/pmm/allocator.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <arch/x64/vmm/scratch.hpp>
#include <arch/x64/vmm/kernel-layout.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <arch/x64/interrupts/idt.hpp>
#include <arch/x64/critical.hpp>
#include <arch/general/failure.hpp>
#include <arch/general/critical.hpp>
#include <cstdint>
#include <iostream>
#include <new>

extern "C" {

void MbootEntry(void * mbootPtr) {
  OS::InitializeOutStream();
  OS::cout << "MbootEntry(" << (uintptr_t)mbootPtr << ")" << OS::endl;
  OS::x64::RegionList::Initialize(mbootPtr);
  
  OS::x64::StepAllocator alloc(OS::x64::KernelSize());
  
  OS::cout << "Initializing memory subsystem..." << OS::endl;
  
  OS::x64::GlobalMap::Initialize(&alloc);
  OS::x64::GlobalMap & map = OS::x64::GlobalMap::GetGlobal();
  OS::x64::Scratch::Initialize(map.GetPDPT(), alloc);
  map.Set();
  
  OS::x64::Allocator::Initialize(alloc);
  map.allocator = &OS::x64::Allocator::GetGlobal();
  
  OS::cout << "Initializing interrupt subsystem..." << OS::endl;
  
  OS::x64::IDT::Initialize();
  OS::x64::IRT::Initialize();
  OS::SetIgnoreCriticality(false);
  OS::SetCritical(false);
  
  OS::Panic("TODO: initialize LAPIC and IOAPIC using ACPI here");
}

/**
 * Only used when not compiling on OS X.
 */
void _MbootEntry(void * mbootPtr) {
  MbootEntry(mbootPtr);
}
  
}
