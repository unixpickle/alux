#ifndef __X64_GLOBAL_MAP_HPP__
#define __X64_GLOBAL_MAP_HPP__

#include <arch/x64/pmm/page-allocator.hpp>
#include <arch/x64/vmm/page-table.hpp>
#include <arch/x64/entry.hpp> // so I can make ::MbootEntry() a friend
#include <arch/general/address-space.hpp>
#include <common>

namespace OS {

namespace x64 {

class GlobalMap : public AddressSpace {
public:
  static void Initialize(PageAllocator * anAlloc);
  static GlobalMap & GetGlobal();
  
  GlobalMap();
  
  PhysAddr GetPDPT();
  
  virtual void Set();
  virtual int GetPageSizeCount();
  virtual size_t GetPageSize(int index);
  virtual size_t GetPageAlignment(int index);
  virtual void Unmap(VirtAddr virt, size_t pageSize, size_t pageCount);
  virtual VirtAddr Map(PhysAddr phys, size_t pageSize, size_t pageCount,
                       bool executable = true);
  virtual void MapAt(VirtAddr virt, PhysAddr phys, size_t pageSize,
                     size_t pageCount, bool executable = true);
  virtual VirtAddr Reserve(size_t pageSize, size_t pageCount);
  
  friend void ::MbootEntry(void *);
  friend class PageTable;
  friend class Allocator;
  
protected:
  PageAllocator * allocator;
  PageTable table;
  PhysAddr pdpt;
  
   // held whenever an allocation is underway
  uint64_t allocationLock OS_ALIGNED(8);
  
  // held whenever the table is being modified directly (inner lock)
  uint64_t tableLock;
  
  VirtAddr freeStart;
  size_t freeSize;
  
  static int PageSizeDepth(size_t size);
  
  void Setup();
  
  VirtAddr AllocateRegion(size_t pageSize, size_t pageCount);
  void FreeRegion(VirtAddr, size_t pageSize, size_t pageCount);
  
  void UpdateFreeRegion();
  bool HasEnoughFree(size_t pageSize, size_t pageCount);
};

}

}

#endif
