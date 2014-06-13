#include <arch/x64/vmm/page-table.hpp>
#include <arch/x64/vmm/scope-scratch.hpp>
#include <arch/x64/vmm/global-map.hpp>
#include <panic>

namespace OS {

namespace x64 {

int PageTable::PageSizeDepth(size_t size) {
  switch (size) {
    case 0x1000:
      return 3;
    case 0x200000:
      return 2;
    case 0x40000000:
      return 1;
    default:
      break;
  }
  Panic("PageTable::PageSizeDepth() - unknown page size");
  return -1;
}

uint64_t PageTable::EntryMask(size_t pageSize, bool exec, bool kernel) {
  return 3 | (pageSize == 0x1000 ? 0 : 0x80) | (kernel ? 0x100 : 0x4)
    | (exec ? 0 : (uint64_t)1 << 63);
}

PageTable::PageTable(PhysAddr base) : pml4(base) {
}

PhysAddr PageTable::GetPML4() {
  return pml4;
}

void PageTable::SetPML4(PhysAddr val) {
  pml4 = val;
}

int PageTable::Walk(VirtAddr addr, uint64_t & entry, size_t * size) {
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
    if (size) *size = 0x1000L << (27 - 9 * depth);
    return depth;
  }
  if (size) *size = 0x1000L << (27 - 9 * depth);
  return -1;
}

bool PageTable::Set(VirtAddr addr,
                    uint64_t entry,
                    uint64_t parentMask,
                    int theDepth) {
  assert(theDepth >= 0 && theDepth < 4);
  assert(!(addr & (0x1000L << (27 - 9 * theDepth)) - 1));
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
      // we have to temporarily leave the critical section, meaning afterwards
      // we will have to clear the scratch's CPU cache
      SetCritical(false);
      nextPage = GlobalMap::GetGlobal().allocator->AllocPage();
      SetCritical(true);
      scratch.InvalidateCache();
      
      scratch[indexes[depth]] = nextPage | parentMask;
      scratch.Reassign(nextPage & 0x7ffffffffffff000);
      
      // zero out the new page
      for (int i = 0; i < 0x200; i++) {
        scratch[i] = 0;
      }
      
      continue;
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
  
  if (addr & (0x1000L << (27 - 9 * maxDepth)) - 1) {
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
    
    SetCritical(false);
    GlobalMap::GetGlobal().allocator->FreePage(tableAddresses[i]);
    SetCritical(true);
    scratch.InvalidateCache();
    
    scratch.Reassign(tableAddresses[i - 1]);
    scratch[indexes[i - 1]] = 0;
  }
  
  return true;
}

}

}
