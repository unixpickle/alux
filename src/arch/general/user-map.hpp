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
   * Reserve mappings starting at the specified address.
   * @noncritical
   */
  virtual void ReserveAt(VirtAddr addr, Size size) = 0;
  
  /**
   * Ensures the security of a certain range. This answers the question: if the
   * kernel tried to access memory in this range, would it be accessing memory
   * that may not belong to the user-space's task? Finding this answer may
   * involve seeing if the memory was ever allocated with a Reserve or Map, or
   * it may simply involve chucking if the range is above some boundary.
   * @noncritical
   */
  virtual bool OwnsRange(VirtAddr start, size_t size) = 0;
  
  /**
   * Delete this instance. You should treat this like the `delete` operator in
   * that you may not access any members of a map after calling it's Delete().
   * @noncritical
   */
  virtual void Delete() = 0;

};

}

#endif
