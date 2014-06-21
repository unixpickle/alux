#ifndef __GENERAL_ADDRESS_SPACE__
#define __GENERAL_ADDRESS_SPACE__

#include <arch-specific/types.hpp>
#include <cstddef>

namespace OS {

class AddressSpace {
public:
  struct Size {
    size_t pageSize;
    size_t pageCount;
    
    Size(size_t ps, size_t pc) : pageSize(ps), pageCount(pc) {
    }
    
    size_t Total() const {
      return pageSize * pageCount;
    }
  };
  
  struct MapInfo : public Size {
    PhysAddr physical;
    bool executable;
    bool writable;
    
    MapInfo(size_t ps, size_t pc, PhysAddr phys, bool exec = true,
            bool write = true)
    : Size(ps, pc), physical(phys), executable(exec), writable(write) {
    }
  };
  
  /**
   * Returns the number of different page sizes supported by this address
   * space.
   * @ambicritical
   */
  static int GetPageSizeCount();
  
  /**
   * Returns the page size at a given index (0 through GetPageSizeCount() - 1
   * inclusive). The page sizes returned from this must be sorted in ascending
   * order, meaning that GetPageSize(0) < GetPageSize(1) ... < GetPageSize(n).
   * @ambicritical
   */
  static size_t GetPageSize(int index);
  
  /**
   * Returns the physical alignment required for pages of a certain size.
   * @ambicritical
   */
  static size_t GetPageAlignment(int index);
  
  static bool ShouldLocateCode();
  static VirtAddr GetCodeLocation();
  
  virtual ~AddressSpace() {}
  
  /**
   * Bind this address space to the current CPU.
   * @critical
   */
  virtual void Set() = 0;
  
  /**
   * Returns whether this address space supports the NX bit.
   * @ambicritical
   */
  virtual bool SupportsNX() = 0;
  
  /**
   * Returns whether this address space supports read-only memory.
   * @ambicritical
   */
  virtual bool SupportsRO() = 0;
  
  /**
   * Returns whether this address space allows Reserve() and MapAt() calls.
   * @ambicritical
   */
  virtual bool SupportsRemap() = 0;
  
  /**
   * Returns whether this address space allows ReserveAt() calls.
   */
  virtual bool SupportsPlacementReserve() = 0;
  
  /**
   * Unmap a chunk of memory from this address space.
   * @noncritical
   */
  virtual void Unmap(VirtAddr virt, Size size) = 0;
  
  /**
   * Map a physical address somewhere into virtual memory and return its
   * address.
   * @noncritical
   */
  virtual VirtAddr Map(MapInfo info) = 0;
  
  /**
   * Map a physical address to a virtual address.
   * @noncritical
   */
  virtual void MapAt(VirtAddr virt, MapInfo info) = 0;
  
  /**
   * Reserve a certain amount of space in the virtual address space. When this
   * memory is written to, a page fault should occur. The only difference
   * between unmapped and reserved memory is that reserved memory will never
   * be returned by a different call to Reserve() or Map() until the reserved
   * memory is unmapped.
   * @noncritical
   */
  virtual VirtAddr Reserve(Size size) = 0;
  
  /**
   * Reserves a certain amount of space at a specified memory location.
   */
  virtual void ReserveAt(VirtAddr virt, Size size) = 0;
  
};

}

#endif
