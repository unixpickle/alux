#ifndef __X64_TASK_HPP__
#define __X64_TASK_HPP__

#include <multitasking/task.hpp>
#include <cstdint>
#include <common>

namespace OS {

namespace x64 {

class Task : public OS::Task {
public:
  typedef OS::Task super;
  
  class Registers {
    uint64_t rsp;
    uint64_t rbp;
    uint64_t rip;
    uint64_t rflags;
    uint64_t cs;
    uint64_t ss;
    uint64_t cr3;
  } OS_PACKED;
  
  Registers regs;
  
  Task(OS::TaskGroup * g);
  virtual void Run();
};

}

}

#endif
