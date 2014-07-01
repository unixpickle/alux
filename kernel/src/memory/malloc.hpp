#ifndef __MEMORY_MALLOC_HPP__
#define __MEMORY_MALLOC_HPP__

#include <memory/region.hpp>
#include <module/module.hpp>

namespace OS {

class Malloc : public Module {
private:
  MallocRegion * first = NULL;
  size_t pageSize;
  size_t pageAlignment;
  size_t allocSize;
  
public:
  static const size_t RegionSize = 0x200000;
  
  static void InitGlobal();
  static Malloc & GetGlobal();
  
  void * Alloc(size_t, bool getNew = true); // @ambicritical, unsynchronized
  void Free(void *); // @ambicritical, unsynchronized
  
protected:
  virtual void Initialize();
  virtual DepList GetDependencies();
  
  void MakeNewRegion();
};

}

#endif
