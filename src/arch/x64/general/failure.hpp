#ifndef __X64_FAILURE_HPP__
#define __X64_FAILURE_HPP__

namespace OS {

/**
 * Call this only once SMP is up and running and interrupts are enabled on all
 * CPUs.
 */
void InitializePanic();

}

#endif
