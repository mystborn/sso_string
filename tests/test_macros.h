#ifndef SSO_STRING_TEST_MACROS_H
#define SSO_STRING_TEST_MACROS_H

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>

#define SSO_STRING_ASSERT_ARG(condition) if(!(condition)) { test_assert_arg_fail = true; return 0; }
#define SSO_STRING_ASSERT_BOUNDS(condition) if(!(condition)) { test_assert_bounds_fail = true; return 0; }

extern bool test_malloc_fail;
extern bool test_assert_arg_fail;
extern bool test_assert_bounds_fail;

static inline void* test_malloc(size_t size) {
    if(test_malloc_fail) 
        return NULL;
    return malloc(size);
}

static inline void* test_realloc(void* src, size_t size) {
    if(test_malloc_fail)
        return NULL;
    return realloc(src, size);
}

#define sso_string_malloc test_malloc
#define sso_string_realloc test_realloc
#define sso_string_free free

#endif // SSO_STRING_TEST_MACROS_H