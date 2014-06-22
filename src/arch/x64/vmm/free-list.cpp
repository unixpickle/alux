#include <arch/x64/vmm/free-list.hpp>
#include <memory/slab.hpp>
#include <lock>
#include <critical>
#include <macros>

#include <iostream> // TODO: delete this

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
  if (start % pageSize) return false;
  
  Region * reg = first;
  Region * last = NULL;
  
  Region requested(start, pageSize * pageCount);
  
  while (reg) {
    if (!reg->Contains(requested)) {
      last = reg;
      reg = reg->next;
      continue;
    }
    AllocInRegion(reg, last, requested);
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
    Region requested(reg->start + chopSize, totalSize);
    AllocInRegion(reg, last, requested);
    return requested.start;
  }
  
  return 0;
}

void FreeList::Free(VirtAddr addr, size_t pageSize, size_t pageCount) {
  AssertNoncritical();
  
  Region * reg = first;
  Region * last = NULL;
  
  Region releasing(addr, pageSize * pageCount);
  
  while (reg) {
    if (reg->IsAdjacentBehind(addr)) {
      // |...|reg|releasing|...|
      
      reg->size += releasing.size;
      while (reg->next) {
        Region * theNext = reg->next;
        if (!reg->IsAdjacentBehind(theNext->start)) break;
        reg->size += theNext->size;
        reg->next = theNext->next;
        FreeRegion(theNext);
      }
      return;
    } else if (releasing.IsAdjacentBehind(reg->start)) {
      // |...|releasing|reg|...|
      
      reg->start -= releasing.size;
      reg->size += releasing.size;
      return;
    } else if (releasing.IsBehind(reg->start)) {
      // |...|releasing|...|reg|...|
      
      break;
    }
    
    last = reg;
    reg = reg->next;
  }
  
  Region * newRegion = AllocRegion(addr, releasing.size);
  newRegion->next = reg;
  if (last) last->next = newRegion;
  else first = newRegion;
}

// FreeList::Region

FreeList::Region::Region(VirtAddr _start, size_t _size)
  : start(_start), size(_size) {
  assert(start + size > start || start + size == 0);
}

VirtAddr FreeList::Region::End() {
  return start + size;
}

bool FreeList::Region::CanHold(size_t pageSize, size_t pageCount) {
  size_t chopSize = 0;
  if (start % pageSize) {
    chopSize += pageSize - (start % pageSize);
  }
  return chopSize + (pageSize * pageCount) <= size;
}

bool FreeList::Region::IsAdjacentBehind(VirtAddr addr) {
  if (start + size == 0) return false;
  return start + size == addr;
}

bool FreeList::Region::Contains(FreeList::Region & reg) {
  if (start > reg.start) return false;
  if (start + size != 0) {
    if (reg.start + reg.size == 0) return false;
    return reg.start + reg.size <= start + size;
  }
  return true;
}

bool FreeList::Region::IsBehind(VirtAddr addr) {
  if (start + size == 0) return false;
  return (start + size <= addr);
}

bool FreeList::Region::IsFilledBy(FreeList::Region & reg) {
  return reg.start == start && reg.size == size;
}

bool FreeList::Region::IsEndedBy(FreeList::Region & reg) {
  return reg.start + reg.size == start + size;
}

bool FreeList::Region::IsStartedBy(FreeList::Region & reg) {
  return reg.start == start;
}

// FreeList private

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
                             Region & requested) {
  if (reg->IsFilledBy(requested)) {
    if (last) last->next = reg->next;
    else first = reg->next;
    FreeRegion(reg);
  } else if (reg->IsStartedBy(requested)) {
    reg->start += requested.size;
  } else if (reg->IsEndedBy(requested)) {
    reg->size -= requested.size;
  } else {
    size_t suffixSize = reg->End() - requested.End();
    Region * suffix = AllocRegion(requested.End(), suffixSize);
    reg->size = requested.start - reg->start;
    suffix->next = reg->next;
    reg->next = suffix;
  }
}

}

}
