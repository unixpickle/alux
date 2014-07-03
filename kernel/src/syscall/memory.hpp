#include <arch/general/arg-list.hpp>
#include <syscall/return-value.hpp>

namespace OS {

ReturnValue SyscallGetPhysicalUsed();
ReturnValue SyscallGetPhysicalAvailable();
ReturnValue SyscallGetPhysicalTotal();

ReturnValue SyscallGetPageSizeCount();
ReturnValue SyscallGetPageSize(ArgList & args);
ReturnValue SyscallGetPageAlignment(ArgList & args);
ReturnValue SyscallAllocatePhysical(ArgList & args);
void SyscallFreePhysical(ArgList & args);
ReturnValue SyscallMapPhysical(ArgList & args);
void SyscallUnmapPhysical(ArgList & args);

}