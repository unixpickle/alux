#include <arch/x64/init.hpp>
#include <arch/general/failure.hpp>
#include <cstdint>
#include <iostream>

extern "C" {

void MbootEntry(void * mbootPtr) {
  OS::InitializeOutStream();
  OS::cout << "MbootEntry(" << (uintptr_t)mbootPtr << ")" << OS::endl;
  
  OS::x64::InitializeMemory(mbootPtr);
  OS::x64::InitializeInterrupts();
  OS::x64::InitializeACPI();
  OS::x64::InitializeAPIC();
  OS::x64::InitializeSMP();
  
  OS::Panic("TODO: platform initialization complete; what do????");
}

/**
 * Only used when compiling with GCC and not Clang.
 */
void _MbootEntry(void * mbootPtr) {
  MbootEntry(mbootPtr);
}
  
}
