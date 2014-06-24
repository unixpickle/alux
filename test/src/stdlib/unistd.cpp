#include <arch/general/syscall.hpp>
#include <cstddef>

extern "C" {

uint64_t getpid() {
  return Syscall(SyscallNumberGetPID, NULL, NULL, NULL, 0, 0);
}

}