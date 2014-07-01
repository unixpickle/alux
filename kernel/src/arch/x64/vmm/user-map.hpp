#ifndef __X64_USER_MAP_HPP__
#define __X64_USER_MAP_HPP__

#include <arch/x64/vmm/page-table.hpp>
#include <arch/x64/vmm/free-list.hpp>
#include <arch/general/user-map.hpp>
#include <macros>

namespace OS {

class Task;

namespace x64 {

/**
 * This manages the structure of a user-space task's memory mapping.
 */
class UserMap : public OS::UserMap {
public:
  static const VirtAddr SpaceStart = 0x8000000000L;
  static const size_t SpaceSize = 0xFF8000000000L;
  
  UserMap();
  ~UserMap();
  
  PhysAddr GetPML4();
  
  virtual void Delete();
  
  virtual void Set();
  virtual bool SupportsNX();
  virtual bool SupportsRO();
  virtual bool SupportsRemap();
  virtual bool SupportsPlacementReserve();
  virtual void Unmap(VirtAddr virt, Size size);
  virtual VirtAddr Map(MapInfo info);
  virtual void MapAt(VirtAddr virt, MapInfo info);
  virtual VirtAddr Reserve(Size size);
  virtual void ReserveAt(VirtAddr addr, Size size);
  virtual bool CopyToKernel(void * dest, VirtAddr start, size_t size);
  virtual bool CopyFromKernel(VirtAddr dest, void * start, size_t size);
  
protected:
  static const size_t RegionVectorJump = 0x10;
  
  /**
   * @critical -> @noncritical -> @critical
   */
  static void FreeTable(PhysAddr table, int depth, int start = 0);
  
  /**
   * @noncritical
   */
  void SetEntries(VirtAddr virt, uint64_t phys, size_t virtAdd,
                  size_t physAdd, size_t count);
  
  /**
   * Tell the TLB to perform TLB invalidations using this map as the "sender."
   * @ambicritical
   */
  void DoInvlpg(VirtAddr virt, size_t size);
  
  uint64_t lock OS_ALIGNED(8) = 0; // @noncritical
  
  PageTable table;
  FreeList freeList;
};

}

}

#endif
