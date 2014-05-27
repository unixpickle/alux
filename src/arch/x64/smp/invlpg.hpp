#ifndef __X64_SMP_INVLPG_HPP__
#define __X64_SMP_INVLPG_HPP__

#include <specific/types.hpp>

namespace OS {

namespace x64 {

/**
 * When a page in the kernel address space is mapped or changed, a simple
 * invlpg is not enough because the other CPUs' caches may still contain stale
 * values. This method runs Invlpg() on the current CPU and runs the equivalent
 * call on all other active CPUs.
 */
void DistributeKernelInvlpg(VirtAddr start, size_t size);

}

}

#endif
