#include <arch/x64/pmm/allocator.hpp>
#include <arch/general/failure.hpp>

namespace OS {

namespace x64 {

static Allocator allocator;

void Allocator::Initialize(StepAllocator & allocator) {
  Panic("OS::x64::Allocator::Initialize() - NYI");
}

Allocator & Allocator::GetGlobal() { 
  return allocator;
}

PhysAddr Allocator::AllocPage() {
  // TODO: NYI
  return 0;
}

void Allocator::FreePage(PhysAddr p) {
  // TODO: NYI
}

}

}