#ifndef _C4_UTIL_HPP_
#define _C4_UTIL_HPP_

#include <stdint.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cinttypes>

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

#define C4_ERROR_NO_ARGS(msg)                                           \
    {                                                                   \
        C4_LOG("\n%s:%d: ERROR: " msg "\nABORTING...\n", __FILE__, __LINE__); \
        std::abort();                                                   \
    }
#define C4_ERROR(msg, ...)                                              \
    {                                                                   \
        C4_LOG("\n%s:%d: ERROR: " msg "\nABORTING...\n", __FILE__, __LINE__, ## __VA_ARGS__); \
        std::abort();                                                   \
    }
#define C4_WARNING(msg, ...) \
    C4_LOG("\n%s:%d: WARNING: " msg "\n", __FILE__, __LINE__, ## __VA_ARGS__);

// error checking - always turned on
/** Check that a condition is true, or raise an error when not true. */
#define C4_CHECK(cond) \
    if(!(cond))\
    {\
        C4_ERROR_NO_ARGS("check failed: " #cond);\
    }
/** like C4_CHECK(), and additionally log a printf-style message.
 * @see C4_CHECK */
#define C4_CHECK_MSG(cond, fmt, ...) \
    if(!(cond))\
    {\
        C4_ERROR("check failed: " #cond "\n" "%s:%d: " fmt, __FILE__, __LINE__, ## __VA_ARGS__);\
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

//------------------------------------------------------------
template< class T > struct fmttag;
template<> struct fmttag<  uint8_t > { static constexpr const char *scn = "%" SCNu8 , *pri = "%" PRIu8 ; };
template<> struct fmttag< uint16_t > { static constexpr const char *scn = "%" SCNu16, *pri = "%" PRIu16; };
template<> struct fmttag< uint32_t > { static constexpr const char *scn = "%" SCNu32, *pri = "%" PRIu32; };
template<> struct fmttag< uint64_t > { static constexpr const char *scn = "%" SCNu64, *pri = "%" PRIu64; };
template<> struct fmttag<   int8_t > { static constexpr const char *scn = "%" SCNd8 , *pri = "%" PRId8 ; };
template<> struct fmttag<  int16_t > { static constexpr const char *scn = "%" SCNd16, *pri = "%" PRId16; };
template<> struct fmttag<  int32_t > { static constexpr const char *scn = "%" SCNd32, *pri = "%" PRId32; };
template<> struct fmttag<  int64_t > { static constexpr const char *scn = "%" SCNd64, *pri = "%" PRId64; };
// floating points... see http://stackoverflow.com/questions/4643641/best-way-to-output-a-full-precision-double-into-a-text-file
template<> struct fmttag<    float > { static constexpr const char *scn = "%g"      , *pri = "%g"      ; };
template<> struct fmttag<   double > { static constexpr const char *scn = "%lg"     , *pri = "%lg"     ; };


template< class T > int fprintc(FILE *file, T const& var) { return fprintf(file, fmttag< T >::pri, var); }
template< class T > int fscanc (FILE *file, T      * var) { return fscanf (file, fmttag< T >::scn, var); }

template< class T > int printc(T const& var) { return printf(stdout, fmttag< T >::pri, var); }
template< class T > int scanc (T      * var) { return scanf (stdin , fmttag< T >::scn, var); }

template< class T > int sprintc(const char *str, T const& var) { return sprintf(str, fmttag< T >::pri, var); }
template< class T > int sscanc (const char *str, T      * var) { return scanf  (str, fmttag< T >::scn, var); }

template< class T > int snprintc(const char *str, size_t sz, T const& var) { return snprintf(str, sz, fmttag< T >::pri, var); }
// there is no snscanf

#endif //_C4_UTIL_HPP_
