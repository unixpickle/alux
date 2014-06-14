#ifndef __MODULE_HPP__
#define __MODULE_HPP__

#include <atomic>
#include <cstddef>

namespace OS {

class Module {
public:
  Module(const Module &) = delete;
  Module(Module &&) = delete;
  Module & operator=(const Module &) = delete;
  Module & operator=(Module &&) = delete;
  
  virtual void Load();
  
  virtual void Initialize() {}
  
protected:
  virtual Module ** GetDependencies(size_t & count) = 0;
  virtual Module ** GetSuperDependencies(size_t & count) = 0;
  
private:
  Atomic<int> loadStartCount;
};

}

#endif
