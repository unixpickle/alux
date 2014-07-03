#ifndef __SYSCALL_HANDLER_HPP__
#define __SYSCALL_HANDLER_HPP__

#include <arch/general/arg-list.hpp>
#include <syscall/return-value.hpp>

namespace OS {

/**
 * The main entry-point for a system call.
 * @critical -> @noncritical (maybe held) -> @critical
 */
ReturnValue SyscallHandler(uint16_t callNumber, ArgList & args);

}

#endif
