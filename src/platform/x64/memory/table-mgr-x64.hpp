#ifndef __PLATFORM_X64_TABLE_MGR_X64_H__
#define __PLATFORM_X64_TABLE_MGR_X64_H__

#include "scope-scratch-x64.hpp"

namespace OS {

namespace x64 {

/**
 * This class handles everything that is 4-level page tables.
 */
class TableMgr {
protected:
  PhysAddr pml4;

public:
  TableMgr(PhysAddr pml4);
  TableMgr();
  
  TableMgr & operator=(const TableMgr & mgr);
  
  /**
   * Return the PML4
   */
  PhysAddr GetPML4();
  
  /**
   * mov cr3, MY_PML4
   */
  void Set();
  
  /**
   * Create a map. This will Panic() if something is already mapped where the
   * region is requested.
   */
  void Map(VirtAddr virt, PhysAddr phys, size_t size, bool largePages,
           uint64_t entryMask, uint64_t tablesMask);
  
  /**
   * Clear the map for a given region or Panic().
   */
  void ClearMap(VirtAddr virt, size_t size);
  
  /**
   * Unmap a region or Panic().
   */
  void Unmap(VirtAddr virt, size_t size);
  
  /**
   * Maps a 4K page.
   */
  void MapSmallPage(VirtAddr virt,
                    PhysAddr phys,
                    uint64_t entryMask,
                    uint64_t tableMask);
  
  /**
   * Maps a 2MB page.
   */
  void MapLargePage(VirtAddr virt,
                    PhysAddr phys,
                    uint64_t entryMask,
                    uint64_t tableMask);
  
  /**
   * Unmap the page starting at addr. The thing is, this page could have been
   * 2MB, or 1GB, or plain old 4K. To deal with this, this function returns the
   * size of the page that was unmapped.
   * @param setToZero If this is false, the page will be set to some
   * non-present but non-zero value (i.e. 0x1000).
   */
  size_t UnmapPage(VirtAddr addr, bool setToZero = true);
  
  /**
   * Examine the status of one or more pages in the mapping. This tells you
   * about consecutive mapped or non-mapped regions.
   */
  void ReadMap(VirtAddr start, size_t & sizeOut, bool & isMapped);
  
  /**
   * Sets the buStart and buSize fields by searching the page tables.
   */
  void FindNewBU(VirtAddr & buStart, size_t & buSize, VirtAddr maxAddr);
  
};

}

}

#endif
