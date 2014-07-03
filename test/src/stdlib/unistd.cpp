#include <arch/general/syscall.hpp>
#include <cstddef>

extern "C" {

uint64_t getpid() {
  return Test::Syscall::Run(Test::Syscall::GetPID, Test::ArgList()).unsigned64;
}

void usleep(uint64_t delay) {
  Test::ArgList list;
  list.PushUInt64(delay);
  Test::Syscall::Run(Test::Syscall::SetTimeout, list);
}

}