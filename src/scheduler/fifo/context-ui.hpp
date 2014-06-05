#ifndef __FIFO_CONTEXT_HPP__
#define __FIFO_CONTEXT_HPP__

namespace OS {

namespace FIFO {

class ContextUI {
public:
  // in the last few seconds or so; these stats will be used to determine if a
  // new Context needs to be activated and assigned some jobs from this one
  uint64_t timeIdle;
  uint64_t timeActive;
};

}

}

#endif
