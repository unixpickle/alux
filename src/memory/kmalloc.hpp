#ifndef __MEMORY_KMALLOC_H__
#define __MEMORY_KMALLOC_H__

#include <platform/failure.hpp>
#include <platform/memory.hpp>
#include <utilities/common.hpp>
#include <utilities/logs.hpp>
#include <utilities/lock.hpp>
#include <analloc2>
#include <stdlib/runtime.hpp>
#include <iostream>

namespace OS {

void InitializeMalloc();

/**
 * @noncritical
 */
void * Malloc(size_t size);

/**
 * @noncritical
 */
void Free(void * ptr);

class MallocRegion : public ANAlloc::Malloc<ANAlloc::BBTree> {
private:
  typedef ANAlloc::Malloc<ANAlloc::BBTree> super;
  static const size_t PageSize = 0x40;

public:
  MallocRegion(void * _start, size_t _length, size_t _used)
    : super(_start, PageSize, _used, _length) {}
  
  MallocRegion * next;
};

}

#endif
