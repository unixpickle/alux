#ifndef __ALUX_PORT_HPP__
#define __ALUX_PORT_HPP__

#include "endpoint.hpp"

namespace Alux {

class Port {
public:
  virtual void Send(const Data & data) = 0;
  virtual bool Receive(Data & data) = 0;
  
  inline int GetControl() {
    return control;
  }
  
  inline void SetControl(int value) {
    control = value;
  }
  
private:
  ansa::Atomic<int> control;
};

}

#endif
