/**
 * Suggestions taken from http://www.gnu.org/software/m68hc11/examples/assert_8h-source.html
 * (the GNU assert.h header)
 */
#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <platform/failure.h>

#ifdef NDEBUG
#define assert(x)
#else
#define assert(x) (void)((x) || (__assert(#x, __FILE__, __LINE__), 0));
#endif

#ifdef __cplusplus
extern "C" {
#endif

void __assert(const char * msg, const char * file, int line);

#ifdef __cplusplus
}
#endif

#endif
