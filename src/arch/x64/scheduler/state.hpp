#ifndef __X64_STATE_HPP__
#define __X64_STATE_HPP__

#include <arch/general/state.hpp>
#include <arch/x64/scheduler/iret-state.hpp>

namespace OS {

namespace x64 {

class State : public OS::State {
public:
  State(void * call, void * arg, bool kernel);
  ~State();
  
  virtual void Load();
  virtual void Delete();
  
private:
  IRETState state;
  uint64_t rdi;
  
  uint8_t * stack;
  bool kernel;
};

}

}

#endif
