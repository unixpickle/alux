#ifndef __PLATFORM_MEMORY_H__
#define __PLATFORM_MEMORY_H__

#include <cstddef>
#include <cstdint>

namespace OS {

typedef uintptr_t PhysAddr;
typedef uintptr_t VirtAddr;

/**
 * Allocate a physically-contiguous chunk of physical memory.
 * @param size The minimum number of bytes to allocate.
 * @param align The alignment to use. Must be a power of 2.
 * @param addr On success, this is the beginning of the physical buffer.
 * @param realSize On success, this is set to the number of usable bytes.
 * @return true on success
 */
bool PhysicalAlloc(size_t size,
                   PhysAddr & addr,
                   size_t * realSize);

/**
 * Like PhysicalAlloc(), but the resulting `addr` is a multiple of `align`.
 */
bool PhysicalAlign(size_t size,
                   size_t align,
                   PhysAddr & addr,
                   size_t * realSize);

bool PhysicalAllocBelow(size_t size,
                        PhysAddr & addr,
                        size_t * realSize,
                        PhysAddr boundary);

bool PhysicalAlignBelow(size_t size,
                        size_t align,
                        PhysAddr & addr,
                        size_t * realSize,
                        PhysAddr boundary);

/**
 * Returns the amount of phyiscal memory currently available for allocation.
 */
size_t PhysicalAvailable();

/**
 * Returns the amount of physical memory that is allocated.
 */
size_t PhysicalUsed();

/**
 * Frees an address allocated with PhysicalAlign() or PhysicalAlloc().
 * @param addr The pointer returned by PhysicalAlign() or PhysicalAlloc().
 */
void PhysicalFree(PhysAddr addr);

namespace KernMap {

  /**
   * @param phys The beginning of the physical segment to map.
   * @param bytes The number of bytes to map. This must be a multiple of a
   * supported page-size.
   * @param addr On success, this will be the beginning of the new virtual
   * buffer that was mapped.
   * @return true on success.
   * @discussion Your implementation of this method ought not to rely on the
   * virtual memory allocation system, because this will be *used* by said
   * system.
   */
  bool Map(PhysAddr phys, size_t bytes, VirtAddr & addr);

  /**
   * Unmap a virtual address but don't let anything else get mapped to it
   * until we actually call Unmap() on the region.
   */
  void MarkBlank(VirtAddr addr, size_t bytes);

  /**
   * Unmap a virtual address that was returned by Map().
   * @param addr The virtual address returned by Map().
   * @param bytes The exact argument you passed for Map()'s `bytes`.
   */
  void Unmap(VirtAddr addr, size_t bytes);

  /**
   * Invalidate the page caches starting at a certain address.
   */
  void InvalidateCache(VirtAddr addr, size_t bytes, size_t pageSize);
  
  /**
   * Returns the number of available page sizes.
   */
  int GetNumPageSizes();
  
  /**
   * Returns the page size at index `idx`.
   */
  size_t GetPageSize(int idx);
  
  /**
   * Returns the required physical alignment for pages of the size at `idx`.
   */
  size_t GetPageAlignment(int idx);
  
}

/**
 * Represents an application's memory map. This memory map includes a section
 * of virtual memory which is shared between every task for the kernel.
 */
class UserMap {
public:
  static int GetNumPageSizes();
  static size_t GetPageSize(int idx);
  static size_t GetPageAlignment(int idx);
  
  static UserMap * CreateUserMap();
  
  /**
   * Destroy a memory map. This will free everything it needs to.
   */
  virtual ~UserMap();
  
  /**
   * Same as MapKernel(), but the mapping will be local to *this* page table.
   */
  bool Map(PhysAddr phys, size_t bytes, VirtAddr & addr);
  
  /**
   * Same as MapUser(), but the mapped pages cannot be executed.
   */
  bool MapNoExec(PhysAddr phys, size_t bytes, VirtAddr & addr);
  
  /**
   * Map non-present pages at a certain address so that a page fault will be
   * triggered when the memory is accessed.
   */
  bool MapReserved(PhysAddr phys, size_t bytes, VirtAddr & addr);
  
  /**
   * Unmap an address but don't let another thing get mapped there until we
   * call Unmap().
   */
  void MarkBlank(VirtAddr addr, size_t bytes);
  
  /**
   * Unmap the pages at a virtual address. The address must be an address
   * returned by one of the Map*() functions, and the size in bytes must be
   * equal to the size passed to the Map*() function.
   */
  void Unmap(VirtAddr addr, size_t bytes);
  
  /**
   * Invalidate the page table caches for a given address extending `bytes`
   * bytes.
   */
  void InvalidateCache(VirtAddr addr, size_t bytes, size_t pageSize);
  
};

}

#endif
