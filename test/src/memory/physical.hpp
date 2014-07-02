#ifndef __MEMORY_PHYSICAL_HPP__
#define __MEMORY_PHYSICAL_HPP__

#include <address.h>
#include <unistd>

namespace std {

PhysAddr AllocatePhysical(size_t pageSize, size_t pageCount);
void FreePhysical(PhysAddr addr)

}

#endif
