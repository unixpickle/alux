#ifndef __X64_GLOBAL_MAP_HPP__
#define __X64_GLOBAL_MAP_HPP__

#include <arch/x64/pmm/page-allocator.hpp>
#include <arch/x64/vmm/page-table.hpp>
#include <arch/general/global-map.hpp>
#include <macros>

namespace OS {

namespace x64 {

class Allocator;
class Scratch;

class GlobalMap : public OS::GlobalMap {
public:
  static void InitGlobal();
  static GlobalMap & GetGlobal();
  
  GlobalMap();
  
  PhysAddr GetPDPT();
  PhysAddr GetPML4();
  
  virtual void Set();
  virtual bool SupportsNX();
  virtual bool SupportsRO();
  virtual bool SupportsRemap();
  virtual bool SupportsPlacementReserve();
  virtual void Unmap(VirtAddr virt, Size size);
  virtual VirtAddr Map(MapInfo info);
  virtual void MapAt(VirtAddr virt, MapInfo info);
  virtual VirtAddr Reserve(Size size);
  virtual void ReserveAt(VirtAddr, Size);
  
  friend class PageTable;
  friend class Allocator;
  friend class Scratch;
  
protected:
  PageAllocator * allocator;
  PageTable table;
  PhysAddr pdpt;
  
  uint64_t lock OS_ALIGNED(8); // @noncritical
  
  VirtAddr freeStart;
  size_t freeSize;
  
  virtual void Initialize();
  virtual DepList GetDependencies();
  virtual DepList GetSuperDependencies();
  
  void Setup();
  
  void SetEntries(VirtAddr virt, uint64_t phys, size_t virtAdd,
                  size_t physAdd, size_t count);
  
  VirtAddr AllocateRegion(Size size);
  void FreeRegion(VirtAddr, Size size);
  
  void UpdateFreeRegion();
  bool HasEnoughFree(Size size);
};

}

}

#endif
