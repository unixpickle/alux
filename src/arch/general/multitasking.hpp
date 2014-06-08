#ifndef __GENERAL_MULTITASKING_HPP__
#define __GENERAL_MULTITASKING_HPP__

#include <multitasking/task.hpp>
#include <multitasking/thread.hpp>

namespace OS {

/**
 * Create a task that may hold kernel threads.
 * @noncritical
 */
OS::Task * CreateKernelTask();

/**
 * Create a kernel thread.
 * @noncritical
 */
OS::Thread * CreateKernelThread(OS::Task *, void * function);

/**
 * Create a kernel thread and pass it an argument.
 * @noncritical
 */
OS::Thread * CreateKernelThread(OS::Task *, void * function, void * argument);

}

#endif