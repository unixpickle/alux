#ifndef __SCHEDULER_IPI_STUB_HPP__
#define __SCHEDULER_IPI_STUB_HPP__

#include <iostream>

namespace OS {

/**
 * Called when an IPI is received on the system.
 * @critical -> @noncritical -> @critical
 */
void * HandleIPI(void * object);

}

#endif
