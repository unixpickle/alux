#ifndef __GENERAL_USER_MAP_HPP__
#define __GENERAL_USER_MAP_HPP__

#include <arch/general/address-space.hpp>

namespace OS {

class UserMap : public AddressSpace {
public:
  /**
   * Create a new UserMap.
   * @noncritical
   */
  static UserMap * New();
  
  /**
   * Returns true if the code should begin at a specific address. If this is
   * the case, call GetCodeLocation() to get the location to ReserveAt() or
   * MapAt() the code.
   * @ambicritical
   */
  static bool ShouldLocateCode();
  
  /**
   * Returns the specific code mapping location if needed.
   * @ambicritical
   */
  static VirtAddr GetCodeLocation();
  
  /**
   * Returns whether this address space allows ReserveAt() calls.
   * @ambicritical
   */
  virtual bool SupportsPlacementReserve() = 0;
  
  /**
   * Repeated from base-class. See address-space.h for more.
   * @noncritical
   */
  virtual VirtAddr Map(MapInfo info) = 0;
  
  /**
   * Same as Map(MapInfo), but a failure will not result in a kernel panic.
   * @noncritical
   */
  virtual bool Map(MapInfo info, VirtAddr & result) = 0;
  
  /**
   * Reserve mappings starting at the specified address.
   * @noncritical
   */
  virtual void ReserveAt(VirtAddr addr, Size size) = 0;
  
  /**
   * Performs a copy from user virtual memory to kernel virtual memory. Here's
   * the thing: a user-space task cannot access kernel memory, but a kernel
   * subroutine definitely can. Thus, we can't use a normal memcpy() with a
   * pointer passed from user-space. Instead, we use this method to make sure
   * that the copy is safe. This method may trigger a normal page fault to
   * signal an illegal access, or it may exit the task using a HoldScope.
   * @noncritical
   */
  virtual void CopyToKernel(void * dest, VirtAddr start, size_t size) = 0;
  
  /**
   * Same as CopyToKernel(), but backwards.
   * @noncritical
   */
  virtual void CopyFromKernel(VirtAddr dest, void * start, size_t size) = 0;
  
  /**
   * Delete this instance. You should treat this like the `delete` operator in
   * that you may not access any members of a map after calling it's Delete().
   * @noncritical
   */
  virtual void Delete() = 0;

};

}

#endif
