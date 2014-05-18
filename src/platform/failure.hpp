#ifndef __PLATFORM_FAILURE_H__
#define __PLATFORM_FAILURE_H__

namespace OS {

/**
 * Halt every other CPU and then display an error message.
 * @ambicritical
 */
void Panic(const char * message);

/**
 * Force the system to shut down completely.
 * @ambicritical
 */
void ForceHalt();

}

#endif
