#include <arch/general/syscall.hpp>

namespace Test {

Syscall::ReturnValue Syscall::Run(uint16_t name, const ArgList & args) {
  Syscall::ReturnValue res;
  uint64_t value;
  __asm__("mov (%%rcx), %%r8\n"
          "mov 8(%%rcx), %%r9\n"
          "syscall"
          : "=a" (value)
          : "D" (name), "S" (args.arguments[0]), "d" (args.arguments[1]),
            "b" (args.arguments[2]), "c" (&args.arguments[3])
          : "memory", "r10", "r11");
  res.unsigned64 = value;
  return res;
}

}
