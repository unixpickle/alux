#ifndef __SCHEDULER_SCHEDULER_INIT_HPP__
#define __SCHEDULER_SCHEDULER_INIT_HPP__

#include <platform/multiprocessing.h>

namespace OS {

bool HasInitializedScheduler();
void InitializeScheduler();

}

#endif
