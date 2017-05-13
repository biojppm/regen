#ifndef _C4_UTIL_HPP_
#define _C4_UTIL_HPP_

#include <stdint.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>

/* NOTE: using , ## __VA_ARGS__ to deal with zero-args calls to
 * variadic macros is not portable, but works in clang, gcc, msvc, icc.
 * clang requires switching off compiler warnings for pedantic mode.
 * @see http://stackoverflow.com/questions/32047685/variadic-macro-without-arguments */
#ifdef __clang__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments" // warning: token pasting of ',' and __VA_ARGS__ is a GNU extension
#endif

#define c4_log std::cout
#define C4_LOG(fmt, ...) std::printf(fmt, ## __VA_ARGS__)

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
// error reporting

#define C4_ERROR(msg, ...)                                              \
    C4_LOG("\n%s:%d: ERROR: " msg "\nABORTING...\n", __FILE__, __LINE__, ## __VA_ARGS__); \
    std::abort();
#define C4_WARNING(msg, ...) \
    C4_LOG("\n%s:%d: WARNING: " msg "\n", __FILE__, __LINE__, ## __VA_ARGS__);

// error checking - always turned on
/** Check that a condition is true, or raise an error when not true. */
#define C4_CHECK(cond) \
    if(!(cond))\
    {\
        C4_ERROR("check failed: ", #cond);\
    }
/** like C4_CHECK(), and additionally log a printf-style message.
 * @see C4_CHECK */
#define C4_CHECK_MSG(cond, fmt, ...) \
    if(!(cond))\
    {\
        C4_ERROR("check failed: " #cond "\n" fmt, ## __VA_ARGS__);\
    }

// assertions - only in debug builds
#ifdef NDEBUG // turn off assertions
#   define C4_ASSERT(cond)
#   define C4_ASSERT_MSG(cond, fmt, ...)
#else
#   define C4_ASSERT(cond) C4_CHECK(cond)
#   define C4_ASSERT_MSG(cond, fmt, ...) C4_CHECK_MSG(cond, fmt, ## __VA_ARGS__)
#endif

// Extreme assertion: can be switched off independently of the regular assertion.
// Use for bounds checking in hot code.
#ifdef C4_USE_XASSERT
#   define C4_XASSERT(cond) C4_ASSERT(cond)
#   define C4_XASSERT_MSG(cond, fmt, ...) C4_ASSERT_MSG(cond, fmt, ## __VA_ARGS__)
#else
#   define C4_XASSERT(cond)
#   define C4_XASSERT_MSG(cond, fmt, ...)
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif //_C4_UTIL_HPP_
