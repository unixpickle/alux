#include "multiboot-x64.hpp"
#include "memory/physical-alloc-x64.hpp"

extern "C" {

void MbootEntry(void * mbootPtr) {
  (void)mbootPtr;
  OS::InitializePrinting();
  OS::InitializeOutStream();
  OS::x64::PhysicalAllocator::Initialize(mbootPtr);
  OS::EntryPoint();
}

void _MbootEntry(void * ptr) {
  MbootEntry(ptr);
}

}
