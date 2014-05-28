#include <arch/x64/pmm/region-list.hpp>
#include <arch/x64/pmm/allocator.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <arch/x64/vmm/scratch.hpp>
#include <arch/x64/vmm/kernel-layout.hpp>
#include <arch/x64/interrupts/irt.hpp>
#include <arch/x64/interrupts/idt.hpp>
#include <arch/x64/acpi/sdt.hpp>
#include <arch/x64/acpi/madt.hpp>
#include <arch/x64/critical.hpp>
#include <arch/general/critical.hpp>
#include <iostream>

namespace OS {

namespace x64 {

void InitializeMemory(void * mbootPtr) {
  cout << "Initializing the memory subsystem..." << endl;
  
  RegionList::Initialize(mbootPtr);
  RegionList & regions = RegionList::GetGlobal();
  cout << "Regions:";
  for (int i = 0; i < regions.GetRegionCount(); i++) {
    MemoryRegion & reg = regions.GetRegions()[i];
    cout << " " << reg.GetStart() << ":" << reg.GetSize();
  }
  cout << endl;
  
  StepAllocator alloc(KernelSize());
  
  GlobalMap::Initialize(&alloc);
  GlobalMap & map = GlobalMap::GetGlobal();
  Scratch::Initialize(map.GetPDPT(), alloc);
  map.Set();
  
  Allocator::Initialize(alloc);
  map.allocator = &Allocator::GetGlobal();
}

void InitializeInterrupts() {
  cout << "Initializing the interrupt subsystem..." << endl;
  
  IDT::Initialize();
  IRT::Initialize();
  SetIgnoreCriticality(false);
  SetCritical(false);
}

void InitializeACPI() {
  cout << "Initializing the ACPI subsystem..." << endl;
  
  SDT::Initialize();
  MADT::Initialize();
}

void InitializeAPIC() {
  
}

void InitializeSMP() {
  
}

}

}
