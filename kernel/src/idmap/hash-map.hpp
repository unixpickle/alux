#ifndef __ALUX_HASH_MAP_HPP__
#define __ALUX_HASH_MAP_HPP__

#include "map.hpp"
#include <anarch/lock>
#include <ansa/atomic>
#include <ansa/linked-list>

namespace Alux {

template <class T, int BucketCount>
class HashMap : public Map<T> {
public:
  virtual void Add(T & object) {
    anarch::ScopedLock scope(lock);
    ++objectCount;
    buckets[object.GetIdentifier() % BucketCount].Add(&object.hashMapLink);
  }
  
  virtual void Remove(T & object) {
    anarch::ScopedLock scope(lock);
    --objectCount;
    buckets[object.GetIdentifier() % BucketCount].Remove(&object.hashMapLink);
  }
  
  virtual T * Find(Identifier anId) {
    anarch::ScopedLock scope(lock);
    const ansa::LinkedList<T> & bucket = buckets[anId % BucketCount];
    for (auto iter = bucket.GetStart(); iter != bucket.GetEnd(); ++iter) {
      T & item = *iter;
      if (item.GetIdentifier() == anId) {
        return &item;
      }
    }
    return NULL;
  }
  
  virtual int GetCount() {
    anarch::ScopedLock scope(lock);
    return objectCount;
  }
  
  virtual bool FindUnusedIds(Identifier upperBound, Identifier & startOut,
                             Identifier & endOut) {
    anarch::ScopedLock scope(lock);
    startOut = endOut = 0;
    for (int i = 0; i < GetBucketCount(); ++i) {
      auto & bucket = GetBucket(i);
      for (auto j = bucket.GetStart(); j != bucket.GetEnd(); ++j) {
        Identifier start = (*j).GetIdentifier() + 1;
        Identifier end = SmallestAfter(start, upperBound);
        if (end - start > endOut - startOut) {
          startOut = start;
          endOut = end;
        }
      }
    }
    return endOut > startOut;
  }
  
  int GetBucketCount() {
    return BucketCount;
  }
  
  ansa::LinkedList<T> & GetBucket(int idx) {
    return buckets[idx];
  }
  
protected:
  anarch::NoncriticalLock lock;
  ansa::LinkedList<T> buckets[BucketCount];
  int objectCount;
  
  Identifier SmallestAfter(Identifier ident, Identifier upperBound) {
    Identifier smallest = upperBound;
    for (int i = 0; i < BucketCount; ++i) {
      auto & bucket = buckets[i];
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
};

}

#endif
