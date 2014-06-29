#include <arch/general/syscall.hpp>
#include <iostream>
#include <cstddef>

namespace std {

void puts(const char * buff, Color color, bool bright) {
  print(buff, color, bright);
  print("\n", color, bright);
}

void print(const char * buff, Color color, bool bright) {
  Syscall(SyscallNumberPrint, (void *)buff, NULL, NULL,
          (uint64_t)color, (uint64_t)bright);           
}

}
