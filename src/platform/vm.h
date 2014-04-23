#ifndef __PLATFORM_VM_H__
#define __PLATFORM_VM_H__

#include <cstdint>

namespace OS {

class VirtualMapping {
private:
  void * tableRoot;

public:
  static uint64_t pageSizes[];
  static uint64_t pageAligns[];
  static uint64_t numPageSizes;

  VirtualMapping(void * tableRoot) {
    this->tableRoot = tableRoot;
  }

  void Unmap(void * address);
  void Map(void * address, void * phys, uint64_t size);
  bool Lookup(void * address, void ** phys);

};

}

#endif
