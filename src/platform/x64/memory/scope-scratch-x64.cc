#include "scope-scratch-x64.hpp"
#include "kernel-map-x64.hpp"

namespace OS {

namespace x64 {

ScopeScratch::ScopeScratch(PhysAddr address) {
  addr = KernelMap::GetGlobal().AllocScratch(address);
  if (!addr) {
    Panic("ScopeScratch::ScopeScratch() - failed to allocate!");
  }
}

ScopeScratch::~ScopeScratch() {
  KernelMap::GetGlobal().FreeScratch(addr);
}

void * ScopeScratch::GetPointer() {
  return (void *)addr;
}

VirtAddr ScopeScratch::GetVirtAddr() {
  return addr;
}

void ScopeScratch::Reassign(PhysAddr newAddr) {
  KernelMap::GetGlobal().ReassignScratch(addr, newAddr);
}

}

}
