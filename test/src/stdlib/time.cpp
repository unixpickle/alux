#include <ctime>
#include <arch/general/syscall.hpp>
#include <cstddef>

extern "C" {

uint64_t utime() {
  return Test::Syscall::Run(Test::Syscall::GetBootMicroTime,
                            Test::ArgList()).unsigned64;
}

}