#include <arch/general/syscall.hpp>
#include <iostream>
#include <cstddef>

namespace std {

void puts(const char * buff) {
  Syscall(SyscallNumberPrint, (void *)buff, NULL, NULL, 0, 0);
}

}
