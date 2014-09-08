#ifndef __ALUX_RETAIN_HASH_MAP_HPP__
#define __ALUX_RETAIN_HASH_MAP_HPP__

#include <anidmap/maps>

namespace Alux {

template <class T, int BucketCount>
class RetainHashMap : public anidmap::HashMap<T, BucketCount> {
public:
  /**
   * Returns a retained object.
   * @noncritical
   */
  virtual T * Find(anidmap::Identifier anId) {
    anarch::ScopedLock scope(this->lock);
    const ansa::LinkedList<T> & bucket = this->GetBucket(anId % BucketCount);
    for (auto iter = bucket.GetStart(); iter != bucket.GetEnd(); ++iter) {
      T & item = *iter;
      if (item.GetIdentifier() == anId) {
        if (!item.Retain()) return NULL;
        return &item;
      }
    }
    return NULL;
  }
};

}

#endif
