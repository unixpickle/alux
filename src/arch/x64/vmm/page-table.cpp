#include <arch/x64/vmm/page-table.hpp>

namespace OS {

namespace x64 {

PageTable::PageTable(PhysAddr base) : pml4(base) {
}

PhysAddr PageTable::GetPML4() {
  return pml4;
}

void PageTable::SetPML4(PhysAddr val) {
  pml4 = val;
}

int PageTable::Walk(VirtAddr addr, uint64_t & entry) {
  // TODO: we need scratch in order to use this
  return -1;
}

void PageTable::Set(VirtAddr addr,
                    uint64_t entry,
                    uint64_t parentMask,
                    int depth) {
  // TODO: we need scratch in order to use this
}

bool PageTable::Unset(VirtAddr addr) {
  // TODO: we need scratch in order to use this
  return false;
}

}

}
