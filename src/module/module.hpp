#ifndef __MODULE_HPP__
#define __MODULE_HPP__

#include <module/dep-list.hpp>
#include <atomic>
#include <cstddef>

namespace OS {

class Module {
public:
  Module();
  ~Module();
  Module(const Module &) = delete;
  Module(Module &&) = delete;
  Module & operator=(const Module &) = delete;
  Module & operator=(Module &&) = delete;
  
  virtual void Load();
  
  virtual void Initialize() {}
  virtual bool IsUninitialized();
  
protected:
  virtual DepList GetDependencies() = 0;
  virtual DepList GetSuperDependencies();
  
private:
  Atomic<int> loadStartCount;
};

}

#endif
