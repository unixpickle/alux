#include <scheduler/base/task.hpp>
#include <scheduler/base/thread.hpp>

namespace OS {

/**
 * @noncritical
 */
Task * CreateKernelTask();

/**
 * @noncritical
 */
Thread * CreateKernelThread(Task * kernTask, void * func);

/**
 * @noncritical
 */
Thread * CreateKernelThread(Task * kernTask, void * func, void * argument);

void SaveAndTick(); // @critical
void SetTimeout(uint64_t delay, bool precision); // @critical
void ClearTimeout(); // @critical
void WaitTimeout(); // @critical -> @noncritical (no return)

}
