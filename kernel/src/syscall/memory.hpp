#include <arch-specific/types.hpp>
#include <cstdint>

namespace OS {

uint64_t SyscallGetPhysicalUsed();
uint64_t SyscallGetPhysicalAvailable();
uint64_t SyscallGetPhysicalTotal();

uint64_t SyscallGetPageSizeCount();
uint64_t SyscallGetPageSize(uint64_t index);
uint64_t SyscallGetPageAlignment(uint64_t index);
uint64_t SyscallAllocatePhysical(PhysAddr * addrOut, uint64_t size,
                                 uint64_t align);
void SyscallFreePhysical(PhysAddr * addrIn);
void SyscallMapPhysical(uint64_t pageSize, uint64_t pageCount,
                        uint64_t * flags, PhysAddr * addrIn,
                        void ** addressOut);
void SyscallUnmapPhysical(uint64_t pageSize, uint64_t pageCount,
                          void * address);

}