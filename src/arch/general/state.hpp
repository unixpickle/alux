#ifndef __GENERAL_STATE_HPP__
#define __GENERAL_STATE_HPP__

namespace OS {

class UserMap;

class State {
public:
  static State * NewKernel(void * call, void * arg); // @noncritical
  static State * NewKernel(void * call); // @noncritical
  static State * NewUser(void * call); // @noncritical
  
  virtual void Load() = 0; // @critical
  virtual void Delete() = 0; // @noncritical
};

}

#endif
