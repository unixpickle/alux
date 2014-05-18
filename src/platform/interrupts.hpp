#ifndef __PLATFORM_INTERRUPTS_H__
#define __PLATFORM_INTERRUPTS_H__

namespace OS {

/**
 * @ambicritical
 */
bool GetInterruptsEnabled();

/**
 * @ambicritical
 */
void SetInterruptsEnabled(bool flag);

}

#endif
