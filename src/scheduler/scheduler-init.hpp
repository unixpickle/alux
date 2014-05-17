#ifndef __SCHEDULER_SCHEDULER_INIT_HPP__
#define __SCHEDULER_SCHEDULER_INIT_HPP__

#include <platform/multiprocessing.hpp>

namespace OS {

bool HasInitializedScheduler();
void InitializeScheduler();

}

#endif
