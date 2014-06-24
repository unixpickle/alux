#include <arch/general/syscall.hpp>
#include <iostream>
#include <cstddef>

namespace std {

void puts(const char * buff, Color color, bool bright) {
  Syscall(SyscallNumberPrint, (void *)buff, NULL, NULL,
          (uint64_t)color, (uint64_t)bright);
  Syscall(SyscallNumberPrint, (void *)"\n", NULL, NULL,
          (uint64_t)color, (uint64_t)bright);
}

}
