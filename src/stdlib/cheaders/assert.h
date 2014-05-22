#ifndef __ASSERT_H__
#define __ASSERT_H__

#ifdef NDEBUG
#define assert(x)
#else
#define ASSERT_STRX(x) #x
#define ASSERT_STR(x) ASSERT_STRX(x)
#define assert(x) (void)((x) ?: (__assert("Assertion failure: " #x " at " __FILE__ ":" ASSERT_STR(__LINE__))));
#endif

#ifdef __cplusplus
extern "C" {
#endif

int __assert(const char * msg);

#ifdef __cplusplus
}
#endif

#endif
