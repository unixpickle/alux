#ifndef __X64_SMP_INVLPG_HPP__
#define __X64_SMP_INVLPG_HPP__

#include <specific/types.hpp>
#include <cstddef>

namespace OS {

namespace x64 {

class InvlpgInfo {
public:
  InvlpgInfo * next;
  VirtAddr start;
  size_t size;
  volatile bool done;
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
 * Set the system up to call invlpg on all other CPUs.
 * @noncritical
 */
void InitializeInvlpg();

}

}

#endif
