#include <arch/x64/pmm/region-list.hpp>
#include <arch/general/failure.hpp>
#include <cstdint>
#include <iostream>

extern "C" {

void MbootEntry(void * mbootPtr) {
  OS::InitializeOutStream();
  OS::cout << "MbootEntry(" << (uintptr_t)mbootPtr << ")" << OS::endl;
  OS::x64::RegionList::Initialize(mbootPtr);
  // TODO: initialize memory management
}

/**
 * Only used when not compiling on OS X.
 */
void _MbootEntry(void * mbootPtr) {
  MbootEntry(mbootPtr);
}
  
}
