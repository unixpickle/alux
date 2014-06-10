#ifndef __MEMORY_PHYSCOPY_HPP__
#define __MEMORY_PHYSCOPY_HPP__

#include <arch-specific/types.hpp>
#include <cstddef>

namespace OS {

/**
 * Temporarily maps a physical address to the virtual address space using an
 * EasyMap, then copies that memory, then unmaps it.
 * @noncritical
 */
void PhysCopy(void * dest, PhysAddr source, size_t size);

}

#endif
