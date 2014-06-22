#ifndef __X64_TLB_HPP__
#define __X64_TLB_HPP__

#include <arch-specific/types.hpp>
#include <module/module.hpp>
#include <cstddef>

namespace OS {

namespace x64 {

class UserMap;

class InvlpgInfo {
public:
  InvlpgInfo * next;
  VirtAddr start;
  size_t size;
  volatile bool done;
  bool isKernel;
};

class TLB : public Module {
public:
  static void InitGlobal();
  static TLB & GetGlobal();
  
  /**
   * Called whenever Set() on any AddressSpace gets called.
   * @critical
   */
  void WillSetUserMap(UserMap * map);
  
  /**
   * Called when the global map triggers page invalidations.
   * @ambicritical
   */
  void InvlpgGlobal(VirtAddr start, size_t size);
  
  /**
   * Called when a user map triggers page invalidations.
   * @ambicritical
   */
  void InvlpgUser(VirtAddr start, size_t size, UserMap * sender);
  
protected:
  virtual void Initialize();
  virtual DepList GetDependencies();
};

}

}

#endif
