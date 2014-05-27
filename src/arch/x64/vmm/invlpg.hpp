#ifndef __X64_VMM_INVLPG_HPP__
#define __X64_VMM_INVLPG_HPP__

#include <specific/types.hpp>
#include <cstddef>

namespace OS {

namespace x64 {

/**
 * Clear the cache on the local CPU.
 */
void Invlpg(VirtAddr start, size_t size);

}

}

#endif
