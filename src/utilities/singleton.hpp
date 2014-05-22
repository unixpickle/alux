#ifndef __UTILITIES_SINGLETON_HPP__
#define __UTILITIES_SINGLETON_HPP__

namespace OS {

class Singleton {
public:
  Singleton() = default;
  Singleton(const Singleton &) = delete;
  Singleton & operator=(const Singleton &) = delete;
};

}

#endif
