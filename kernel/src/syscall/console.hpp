#ifndef __SYSCALL_CONSOLE_HPP__
#define __SYSCALL_CONSOLE_HPP__

#include <arch/general/arg-list.hpp>

namespace OS {

void SyscallPrint(ArgList & list);

}

#endif
