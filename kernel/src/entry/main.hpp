#ifndef __MAIN_HPP__
#define __MAIN_HPP__

#include <module/module.hpp>

namespace OS {

class MainModule : public Module {
public:
  static void InitGlobal();
  static MainModule & GetGlobal();
  
  virtual void Initialize();
  virtual DepList GetDependencies();
  
  void Main();
};

}

#endif
