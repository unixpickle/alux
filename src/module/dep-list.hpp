#ifndef __MODULE_DEP_LIST_HPP__
#define __MODULE_DEP_LIST_HPP__

namespace OS {

class Module;

struct DepList {
  static const int MaxDeps = 0x10;
  
  Module * modules[MaxDeps];
  int count;
  
  template <typename... Args>
  DepList(Args... args) : count(0) {
    SetArgs(args...);
  }
  
  DepList(const DepList & list) {
    *this = list;
  }
  
  DepList & operator=(const DepList & list) {
    count = list.count;
    for (int i = 0; i < count; i++) {
      modules[i] = list[i];
    }
  }
  
  Module *& operator[](int idx) const {
    return modules[idx];
  }
  
private:
  template <typename T, typename... Args>
  void SetArgs(T anArg, Args... args) {
    assert(count < MaxDeps);
    modules[count++] = anArg;
    SetArgs(args...);
  }
  
  void SetArgs() {
  }
};

}

#endif
