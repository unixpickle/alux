#ifndef __X64_GLOBAL_MAP_HPP__
#define __X64_GLOBAL_MAP_HPP__

#include <arch/x64/pmm/page-allocator.hpp>
#include <arch/x64/vmm/page-table.hpp>
#include <arch/x64/init.hpp> // so I can make InitializeMemory() a friend
#include <arch/general/address-space.hpp>
#include <macros>

namespace OS {

namespace x64 {

class GlobalMap : public AddressSpace {
public:
  static void Initialize(PageAllocator * anAlloc);
  static GlobalMap & GetGlobal();
  
  GlobalMap();
  
  PhysAddr GetPDPT();
  PhysAddr GetPML4();
  
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
  
  friend void InitializeMemory(void *);
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
  
  void Setup();
  
  void SetEntries(VirtAddr virt, uint64_t phys, size_t virtAdd,
                  size_t physAdd, size_t count);
  
  VirtAddr AllocateRegion(size_t pageSize, size_t pageCount);
  void FreeRegion(VirtAddr, size_t pageSize, size_t pageCount);
  
  void UpdateFreeRegion();
  bool HasEnoughFree(size_t pageSize, size_t pageCount);
};

}

}

#endif
