#ifndef __GENERAL_ADDRESS_SPACE__
#define __GENERAL_ADDRESS_SPACE__

#include <specific/types.hpp>

namespace OS {

class AddressSpace {
public:
  static AddressSpace & GetGlobal();
  
  virtual ~AddressSpace() {}
  
  /**
   * Bind this address space to the current CPU.
   * @critical
   */
  virtual void Set() = 0;
  
  /**
   * Returns the number of different page sizes supported by this address
   * space.
   * @ambicritical
   */
  virtual int GetPageSizeCount() = 0;
  
  /**
   * Returns the page size at a given index (0 through GetPageSizeCount() - 1
   * inclusive). The page sizes returned from this must be sorted in ascending
   * order, meaning that GetPageSize(0) < GetPageSize(1) ... < GetPageSize(n).
   * @ambicritical
   */
  virtual size_t GetPageSize(int index) = 0;
  
  /**
   * Returns the physical alignment required for pages of a certain size.
   */
  virtual size_t GetPageAlignment(int index) = 0;
  
  /**
   * Unmap a chunk of memory from this address space.
   * @noncritical
   */
  virtual void Unmap(VirtAddr virt, size_t pageSize, size_t pageCount) = 0;
  
  /**
   * Map a physical address somewhere into virtual memory and return its
   * address.
   * @noncritical
   */
  virtual VirtAddr Map(PhysAddr phys, size_t pageSize, size_t pageCount,
                       bool executable = true) = 0;
  
  /**
   * Map a physical address to a virtual address.
   * @noncritical
   */
  virtual void MapAt(VirtAddr virt, PhysAddr phys, size_t pageSize,
                     size_t pageCount, bool executable = true) = 0;
  
  /**
   * Reserve a certain amount of space in the virtual address space. When this
   * memory is written to, a page fault should occur. The only difference
   * between unmapped and reserved memory is that reserved memory will never
   * be returned by a different call to Reserve() or Map() until the reserved
   * memory is unmapped.
   * @noncritical
   */
  virtual VirtAddr Reserve(size_t pageSize, size_t pageCount) = 0;
  
};

}

#endif
