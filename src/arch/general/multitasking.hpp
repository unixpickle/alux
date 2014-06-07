#ifndef __GENERAL_MULTITASKING_HPP__
#define __GENERAL_MULTITASKING_HPP__

#include <multitasking/task-group.hpp>
#include <multitasking/task.hpp>

namespace OS {

/**
 * Create a task group that may hold kernel tasks.
 * @noncritical
 */
TaskGroup * CreateKernelTaskGroup();

/**
 * Create a kernel task for the created task group.
 * @noncritical
 */
Task * CreateKernelTask(TaskGroup * group, void * function);

/**
 * Create a kernel task for the created task group and pass an argument to it.
 * @noncritical
 */
Task * CreateKernelTask(TaskGroup * group, void * function, void * argument);

}

#endif