#include <arch/x64/vmm/scope-scratch.hpp>
#include <arch/x64/vmm/scratch.hpp>
#include <arch/general/failure.hpp>

namespace OS {

namespace x64 {

ScopeScratch::ScopeScratch(PhysAddr address) {
  ptr = Scratch::GetGlobal().Alloc(address);
  if (!ptr) Panic("failed to allocate scratch");
}

ScopeScratch::~ScopeScratch() {
  Scratch::GetGlobal().Free(ptr);
}
  
void * ScopeScratch::GetPointer() {
  return (void *)ptr;
}

VirtAddr ScopeScratch::GetVirtAddr() {
  return ptr;
}

void ScopeScratch::Reassign(PhysAddr newAddr) {
  Scratch::GetGlobal().Reassign(ptr, newAddr);
}

}

}
