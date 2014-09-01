#ifndef __ALUX_STEP_ID_MAP_HPP__
#define __ALUX_STEP_ID_MAP_HPP__

#include "id-map.hpp"

namespace Alux {

/**
 * Allocate identifiers incrementally up to but not including a specified
 * limit. Both allocating and freeing run in amortized O(1) time.
 *
 * In the case when identifiers wrap around and the allocator needs to find a
 * new chunk of free identifiers, an allocation runs in O(n^2) time.
 */
template <class T, int Buckets>
class StepIdMap : public IdMap<T, Buckets> {
public:
  /**
   * Create an instance with an [upperBound].
   * @ambicritical
   */
  StepIdMap(Identifier b) : upperBound(b), freeEnd(b) {
  }
  
protected:
  virtual bool Alloc(Identifier & identifier) {
    if (freeStart == freeEnd) {
      FindFree();
      if (freeStart == freeEnd) return false;
    }
    identifier = freeStart++;
    return true;
  }
  
  virtual void Free(Identifier identifier) {
    // in the off-chance that they're freeing an identifier right on the edge
    // of our free region
    if (identifier + 1 == freeStart) {
      freeStart = identifier;
    } else if (identifier == freeEnd) {
      freeEnd = identifier;
    }
  }
  
private:
  void FindFree() {
    AssertNoncritical();
    freeStart = freeEnd = 0;
    for (int i = 0; i < GetBucketCount(); ++i) {
      auto & bucket = GetBucket(i);
      for (auto j = bucket.GetStart(); j != bucket.GetEnd(); ++j) {
        Identifier start = (*j).GetIdentifier() + 1;
        Identifier end = SmallestAfter(anId, upperBound);
        if (end - start > freeEnd - freeStart) {
          freeStart = start;
          freeEnd = end;
        }
      }
    }
  }
  
  Identifier SmallestAfter(Identifier ident) {
    Identifier smallest = upperBound;
    for (int i = 0; i < GetBucketCount(); ++i) {
      auto & bucket = GetBucket(i);
      for (auto j = bucket.GetStart(); j != bucket.GetEnd(); ++j) {
        Identifier anId = (*j).GetIdentifier();
        if (anId == ident) return anId;
        if (anId < smallest && anId > ident) {
          smallest = anId;
        }
      }
    }
    return smallest;
  }
  
private:
  Identifier upperBound;
  Identifier freeStart = 0;
  Identifier freeEnd;
};

}

#endif
