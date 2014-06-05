#ifndef __FIFO_JOB_UI_HPP__
#define __FIFO_JOB_UI_HPP__

#include <scheduler/fifo/job-list.hpp>

namespace OS {

class Context;

namespace FIFO {

class JobUI {
public:
  JobList::Node node;
  Context * pinContext;
};

}

}

#endif
