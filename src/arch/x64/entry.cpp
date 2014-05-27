#include <arch/x64/pmm/region-list.hpp>
#include <arch/x64/pmm/allocator.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <arch/x64/vmm/scratch.hpp>
#include <arch/x64/vmm/kernel-layout.hpp>
#include <arch/general/failure.hpp>
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
  
  uint8_t * ptr = new uint8_t[0x20];
  OS::cout << "ptr 1 " << (uintptr_t)ptr << OS::endl;
  uint8_t * buffer = new uint8_t[0x2000];
  OS::cout << "ptr 2 " << (uintptr_t)buffer << OS::endl;
  delete ptr;
  delete buffer;
  
  OS::Panic("TODO: initialize interrupts after memory");
}

/**
 * Only used when not compiling on OS X.
 */
void _MbootEntry(void * mbootPtr) {
  MbootEntry(mbootPtr);
}
  
}
