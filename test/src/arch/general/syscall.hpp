#ifndef __GENERAL_SYSCALL_HPP__
#define __GENERAL_SYSCALL_HPP__

#include <arch-specific/arg-list.hpp>

namespace Test {

class Syscall {
public:
  enum Identifier {
    // console
    Print,
    // threading
    LaunchThread,
    Fork,
    Exit,
    ThreadExit,
    GetPID,
    GetThreadID,
    // time
    GetBootMicroTime,
    SetTimeout,
    SetInfiniteTimeout,
    ClearTimeout,
    ClearClear,
    // thread/task stats
    GetTaskCount,
    GetThreadCount,
    // memory
    GetPhysicalUsed,
    GetPhysicalAvailable,
    GetPhysicalTotal,
    GetPageSizeCount,
    GetPageSize,
    GetPageAlignment,
    AllocatePhysical,
    FreePhysical,
    MapPhysical,
    UnmapPhysical
  };
  
  typedef union {
    bool boolean;
    int integer;
    uint32_t unsigned32;
    uint64_t unsigned64;
    PhysAddr physicalAddr;
    VirtAddr virtualAddr;
  } ReturnValue;
  
  static_assert(sizeof(ReturnValue) == 8, "invalid ReturnValue size");
  
  static ReturnValue Run(uint16_t name, const ArgList & args);
};

}

#endif
