#include <ctime>
#include <arch/general/syscall.hpp>
#include <cstddef>

extern "C" {

uint64_t utime() {
  return Syscall(SyscallNumberGetBootMicroTime, NULL, NULL, NULL, 0, 0);
}

}