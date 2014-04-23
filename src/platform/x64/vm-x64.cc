#include <platform/vm.h>

namespace OS {

static uintptr_t pageSizes[] = {0x1000, 0x200000, 0x40000000};
uintptr_t * VirtualMapping::pageSizes = pageSizes;
uintptr_t * VirtualMapping::physicalAligns = pageSizes;
uintptr_t * VirtualMapping::virtualAligns = pageSizes;
int VirtualMapping::numPageSizes = 3;

void VirtualMapping::Unmap(void * address) {
  // here, use our standard x64 page table logic to unmap the address
}

bool VirtualMapping::Map(void * address, void * phys, uint64_t size) {
  // here, map the address using standard x64 page table logic
  return false;
}

bool Lookup(void * address, void ** phys, void ** start, uint64_t * size) {
  // here, use the typical x64 page table walk
  return false;
}

}
