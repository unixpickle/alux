#ifndef __MULTITASKING_FUNCTIONS_HPP__
#define __MULTITASKING_FUNCTIONS_HPP__

#include <multitasking/thread.hpp>

namespace OS {

/**
 * @noncritical
 */
void SetupTask(Task * t);

/**
 * @noncritical
 */
void LaunchThread(Thread * th);

/**
 * @critical
 */
void ResignThread();

}

#endif