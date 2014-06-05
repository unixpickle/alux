#ifndef __FIFO_TIMER_HPP__
#define __FIFO_TIMER_HPP__

namespace OS {

namespace FIFO {

class Timer {
public:
  uint64_t fireTime;
  Job * job;
  bool precision;
};

}

}

#endif
