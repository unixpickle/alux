#ifndef __SCHEDULER_DEFAULT_HPP__
#define __SCHEDULER_DEFAULT_HPP__

#include <scheduler/fifo/task-info.hpp>
#include <scheduler/fifo/thread-info.hpp>

namespace OS {

typedef FIFOScheduler::TaskInfo TaskInfo;
typedef FIFOScheduler::ThreadInfo ThreadInfo;

}

#endif