#include "step-id-allocator.hpp"

namespace Alux {

StepIdAllocator::StepIdAllocator(UnusedIdFinder & m, Identifier b)
  : map(m), upperBound(b) {
  if (!map.FindUnusedIds(upperBound, freeStart, freeEnd)) {
    freeStart = freeEnd = 0;
  }
}

bool StepIdAllocator::Alloc(Identifier & identifier) {
  anarch::ScopedLock scope(lock);
  if (freeStart == freeEnd) {
    if (!map.FindUnusedIds(upperBound, freeStart, freeEnd)) {
      return false;
    }
  }
  identifier = freeStart++;
  return true;
}

bool StepIdAllocator::Alloc(Identifier & identifier, int max) {
  anarch::ScopedLock scope(lock);
  if (count >= max) return false;
  if (freeStart == freeEnd) {
    if (!map.FindUnusedIds(upperBound, freeStart, freeEnd)) {
      return false;
    }
  }
  identifier = freeStart++;
  ++count;
  return true;
}

void StepIdAllocator::Free(Identifier identifier) {
  anarch::ScopedLock scope(lock);
  --count;
  
  // in the off-chance that they're freeing an identifier right on the edge
  // of our free region, we can expand the region.
  if (identifier + 1 == freeStart) {
    freeStart = identifier;
  } else if (identifier == freeEnd) {
    freeEnd = identifier;
  }
}

int StepIdAllocator::GetCount() {
  anarch::ScopedLock scope(lock);
  return count;
}

}
