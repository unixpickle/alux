#include <arch/x64/init.hpp>
#include <panic>
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
  OS::x64::InitializeTime();
  OS::x64::InitializeSyscall();
  OS::x64::InitializeSMP();
  OS::x64::InitializeTimers();
  OS::x64::InitializeScheduler();
  
  OS::Panic("OS::x64::InitializeScheduler() shouldn't return");
}

/**
 * Only used when compiling with GCC and not Clang.
 */
void _MbootEntry(void * mbootPtr) {
  MbootEntry(mbootPtr);
}
  
}
