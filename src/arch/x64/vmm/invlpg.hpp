#ifndef __X64_VMM_INVLPG_HPP__
#define __X64_VMM_INVLPG_HPP__

#include <arch-specific/types.hpp>
#include <cstddef>

namespace OS {

namespace x64 {

/**
 * Clear the cache on the local CPU.
 * @critical
 */
void Invlpg(VirtAddr start, size_t size, bool kernel = true);

}

}

#endif
