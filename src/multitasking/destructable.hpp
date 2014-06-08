#ifndef __UTILITIES_DESTRUCTABLE_HPP__
#define __UTILITIES_DESTRUCTABLE_HPP__

namespace OS {

class GarbageThread;

class Destructable {
public:
  virtual ~Destructable() {}

protected:
  Destructable * nextDestructable;
  Destructable * lastDestructable;
  friend class GarbageThread;
};

}

#endif
