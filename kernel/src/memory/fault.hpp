#ifndef __MEMORY_FAULT_HPP__
#define __MEMORY_FAULT_HPP__

#include <arch-specific/types.hpp>

namespace OS {

/**
 * Called when a page fault occurs.
 * @critical -> @noncritical -> @critical
 */
void HandleMemoryFault(VirtAddr addr, bool exec, bool write);

}

#endif
