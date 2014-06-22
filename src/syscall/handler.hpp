#ifndef __SYSCALL_HANDLER_HPP__
#define __SYSCALL_HANDLER_HPP__

#include <cstdint>

namespace OS {

/**
 * The main entry-point for a system call.
 * @critical -> @noncritical (maybe held) -> @critical
 */
void SyscallHandler(uint16_t callNumber,
                    void * arg1,
                    void * arg2,
                    void * arg3,
                    uint64_t arg4,
                    uint64_t arg5);

}

#endif
