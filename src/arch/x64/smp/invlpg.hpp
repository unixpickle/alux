#ifndef __X64_SMP_INVLPG_HPP__
#define __X64_SMP_INVLPG_HPP__

#include <arch-specific/types.hpp>
#include <module/module.hpp>
#include <cstddef>

namespace OS {

class Task;

namespace x64 {

class InvlpgInfo {
public:
  InvlpgInfo * next;
  VirtAddr start;
  size_t size;
  volatile bool done;
  bool isKernel;
};

class InvlpgModule : public Module {
public:
  static void InitGlobal();
  static InvlpgModule & GetGlobal();
  
  virtual void Initialize();
  virtual DepList GetDependencies();
  
  bool IsInitialized();
  
private:
  volatile bool isInitialized;
};

/**
 * When a page in the kernel address space is mapped or changed, a simple
 * invlpg is not enough because the other CPUs' caches may still contain stale
 * values. This method runs Invlpg() on the current CPU and runs the equivalent
 * call on all other active CPUs.
 * @ambicritical
 */
void DistributeKernelInvlpg(VirtAddr start, size_t size);

/**
 * Tell every CPU running a task, including ourselves, to invalidate a certain
 * region of cache.
 * @ambicritical
 */
void DistributeUserInvlpg(VirtAddr start, size_t size, Task * t);

}

}

#endif
