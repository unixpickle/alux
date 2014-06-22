#include <arch/x64/vmm/free-list.hpp>
#include <memory/slab.hpp>
#include <lock>
#include <critical>
#include <macros>

namespace OS {

namespace x64 {

typedef Slab<FreeList::Region, 0x10000 / sizeof(FreeList::Region)> SlabType;

static uint64_t slabInitLock OS_ALIGNED(8);
static bool slabInitialized;
static SlabType slab;

FreeList::~FreeList() {
  Region * reg = first;
  while (reg) {
    Region * next = reg->next;
    FreeRegion(reg);
    reg = next;
  }
}

bool FreeList::AllocAt(VirtAddr start, size_t pageSize, size_t pageCount) {
  Region * reg = first;
  Region * last = NULL;
  size_t size = pageSize * pageCount;
  
  if (start % pageSize) return false;
  
  while (reg) {
    if (reg->start > start || reg->start + reg->size < start + size) {
      last = reg;
      reg = reg->next;
      continue;
    }
    size_t chopSize = start - reg->start;
    AllocInRegion(reg, last, chopSize, size);
    return true;
  }
  return false;
}

VirtAddr FreeList::Alloc(size_t pageSize, size_t pageCount) {
  AssertNoncritical();
  Region * last = NULL;
  Region * reg = first;
  
  size_t totalSize = pageSize * pageCount;
  
  while (reg) {
    if (!reg->CanHold(pageSize, pageCount)) {
      last = reg;
      reg = reg->next;
      continue;
    }
    
    size_t chopSize = 0;
    if (reg->start % pageSize) {
      chopSize = pageSize - (reg->start % pageSize);
    }
    VirtAddr result = reg->start + chopSize;
    AllocInRegion(reg, last, chopSize, totalSize);
    return result;
  }
  
  return 0;
}

void FreeList::Free(VirtAddr addr, size_t pageSize, size_t pageCount) {
  AssertNoncritical();
  
  Region * reg = first;
  Region * last = NULL;
  size_t size = pageSize * pageCount;
  
  while (reg) {
    if (reg->start == addr + size) {
      reg->start -= size;
      reg->size += size;
      return;
    } else if (reg->IsAdjacentBehind(addr)) {
      reg->size += size;
      while (reg->next) {
        Region * theNext = reg->next;
        if (!reg->IsAdjacentBehind(theNext->start)) break;
        reg->size += theNext->size;
        reg->next = theNext->next;
        FreeRegion(theNext);
      }
      return;
    } else if (addr > reg->start + reg->size) {
      break;
    }
    
    last = reg;
    reg = reg->next;
  }
  
  Region * newRegion = AllocRegion(addr, size);
  if (last) {
    last->next = newRegion;
  } else {
    first = newRegion;
  }
}

FreeList::Region::Region(VirtAddr _start, size_t _size)
  : start(_start), size(_size) {
}

bool FreeList::Region::CanHold(size_t pageSize, size_t pageCount) {
  size_t chopSize = 0;
  if (start % pageSize) {
    chopSize += pageSize - (start % pageSize);
  }
  return chopSize + (pageSize * pageCount) <= size;
}

bool FreeList::Region::IsAdjacentBehind(VirtAddr addr) {
  return start + size == addr;
}

FreeList::Region * FreeList::AllocRegion(VirtAddr start, size_t size) {
  AssertNoncritical();
  if (!slabInitialized) {
    ScopeLock scope(&slabInitLock);
    if (!slabInitialized) {
      slabInitialized = true;
      new(&slab) SlabType();
    }
  }
  return slab.New(start, size);
}

void FreeList::FreeRegion(FreeList::Region * reg) {
  AssertNoncritical();
  slab.Delete(reg);
}

void FreeList::AllocInRegion(Region * reg, Region * last,
                             size_t chopSize, size_t totalSize) {
  VirtAddr result = reg->start + chopSize;
  if (reg->start + chopSize + totalSize == reg->size) {
    // we are totally removing the end of the chunk
    if (chopSize) {
      reg->size = chopSize;
    } else {
      if (last) last->next = reg->next;
      else first = reg->next;
      FreeRegion(reg);
    }
  } else {
    // we only need to remove a piece of the chunk
    if (chopSize) {
      Region * chopRegion = AllocRegion(reg->start, chopSize);
      chopRegion->next = reg;
      if (last) last->next = chopRegion;
      else first = chopRegion;
    }
    reg->size -= totalSize + chopSize;
    reg->start += totalSize + chopSize;
  }
}

}

}
