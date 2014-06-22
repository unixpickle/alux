#ifndef __X64_IRET_STATE_HPP__
#define __X64_IRET_STATE_HPP__

#include <cstdint>
#include <macros>

namespace OS {

namespace x64 {

struct IRETState {
  // these are ordered this way for iretq
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
} OS_PACKED;

}

}

#endif
