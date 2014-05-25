#ifndef __X64_PAGE_TABLE_HPP__
#define __X64_PAGE_TABLE_HPP__

#include <specific/types.hpp>
#include <cstdint>

namespace OS {

namespace x64 {

class PageTable {
private:
  PhysAddr pml4;

public:
  PageTable(PhysAddr pml4);
  
  PhysAddr GetPML4();
  
  void SetPML4(PhysAddr pml4);
  
  /**
   * @return The depth of the entry found (0-3 inclusive) or -1 if not mapped.
   */
  int Walk(VirtAddr addr, uint64_t & entry);
  
  /**
   * Set a raw entry in the page table.
   * @param addr The start of the page
   * @param entry The entry to set, flags included
   * @param parentMask All parent table entries will be ORed with this
   * @param depth The depth of the entry to create; 0-3 inclusive.
   */
  void Set(VirtAddr addr, uint64_t entry, uint64_t parentMask, int depth);
  
  /**
   * Unset the page table entry beginning at `addr` or return false if no entry
   * exists exactly at that address.
   */
  bool Unset(VirtAddr addr);
  
};

}

}

#endif
