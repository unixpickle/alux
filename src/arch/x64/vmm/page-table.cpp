#include <arch/x64/vmm/page-table.hpp>
#include <arch/x64/vmm/scope-scratch.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <arch/general/failure.hpp>

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
  int indexes[4] = {
    (int)((addr >> 39) & 0x1ff),
    (int)((addr >> 30) & 0x1ff),
    (int)((addr >> 21) & 0x1ff),
    (int)((addr >> 12) & 0x1ff)
  };
  TypedScratch<uint64_t> scratch(pml4);
  int depth;
  for (depth = 0; depth < 3; depth++) {
    uint64_t nextPage = scratch[indexes[depth]];
    if (nextPage & 0x80 || !(nextPage & 1)) {
      break;
    }
    scratch.Reassign(nextPage & 0x7ffffffffffff000);
  }
  if (scratch[indexes[depth]] & 0x80 || depth == 3) {
    entry = scratch[indexes[depth]];
    return depth;
  }
  return -1;
}

bool PageTable::Set(VirtAddr addr,
                    uint64_t entry,
                    uint64_t parentMask,
                    int theDepth) {
  assert(theDepth >= 0 && theDepth < 4);
  assert(!(addr & (1L << (39 - 9 * theDepth)) - 1));
  int indexes[4] = {
    (int)((addr >> 39) & 0x1ff),
    (int)((addr >> 30) & 0x1ff),
    (int)((addr >> 21) & 0x1ff),
    (int)((addr >> 12) & 0x1ff)
  };
  
  TypedScratch<uint64_t> scratch(pml4);
  for (int depth = 0; depth < theDepth; depth++) {
    uint64_t nextPage = scratch[indexes[depth]];
    if (!(nextPage & 1)) {
      nextPage = GlobalMap::GetGlobal().allocator->AllocPage();
      
      // we need to zero out the newly allocated table for security etc.
      TypedScratch<uint64_t> temp(nextPage);
      for (int i = 0; i < 0x200; i++) {
        temp[i] = 0;
      }
      
      scratch[indexes[depth]] = nextPage | parentMask;
    } else if (nextPage & 0x80) {
      return false;
    } else {
      scratch[indexes[depth]] |= parentMask;
    }
    scratch.Reassign(nextPage & 0x7ffffffffffff000);
  }
  
  scratch[indexes[theDepth]] = entry;
  return true;
}

bool PageTable::Unset(VirtAddr addr) {
  assert(!(addr & 0xfff));
  int indexes[4] = {
    (int)((addr >> 39) & 0x1ff),
    (int)((addr >> 30) & 0x1ff),
    (int)((addr >> 21) & 0x1ff),
    (int)((addr >> 12) & 0x1ff)
  };
  
  PhysAddr tableAddresses[4] = {pml4, 0, 0, 0};
  
  TypedScratch<uint64_t> scratch(pml4);
  int maxDepth = 3;
  for (int depth = 0; depth < 3; depth++) {
    uint64_t nextPage = scratch[indexes[depth]];
    if (nextPage & 0x80) {
      maxDepth = depth;
      break;
    } else if (!(nextPage & 1)) {
      Panic("TableMgr::UnmapPage() - page not mapped!");
    }
    tableAddresses[depth + 1] = nextPage & 0x7ffffffffffff000;
    scratch.Reassign(tableAddresses[depth + 1]);
  }
  
  if (addr & (1L << (39 - 9 * maxDepth)) - 1) {
    // the address is not properly aligned to the page we found
    return false;
  }
  
  // zero out the page and delete any parent table indexes
  scratch[indexes[maxDepth]] = 0;

  for (int i = maxDepth; i > 0; i--) {
    bool allGone = true;
    for (int j = 0; j < 0x200; j++) {
      if (scratch[j]) {
        allGone = false;
        break;
      }
    }
    if (!allGone) break;
    
    GlobalMap::GetGlobal().allocator->FreePage(tableAddresses[i]);
    scratch.Reassign(tableAddresses[i - 1]);
    scratch[indexes[i - 1]] = 0;
  }
  
  return true;
}

}

}
