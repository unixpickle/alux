#include "multiboot-x64.hpp"
#include "memory/pmm-x64.hpp"

extern "C" {

void MbootEntry(void * mbootPtr) {
  (void)mbootPtr;
  OS::InitializePrinting();
  OS::InitializeOutStream();
  OS::x64::PMM::Initialize(mbootPtr);
  OS::EntryPoint();
}

void _MbootEntry(void * ptr) {
  MbootEntry(ptr);
}

}
