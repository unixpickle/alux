#include <cstdint>
#include <utilities/common.hpp>
#include <utilities/critical.hpp>

namespace OS {

namespace x64 {

/**
 * @ambicritical
 */
void OutB(uint16_t port, uint8_t byte);

/**
 * @critical
 */
void CPUID(uint32_t eax, uint32_t * ebx, uint32_t * edx, uint32_t * ecx);

/**
 * @critical
 */
uint64_t ReadMSR(uint32_t cell);

/**
 * @critical
 */
void WriteMSR(uint32_t cell, uint64_t value);

}

}

