#ifndef _C4_UTIL_HPP_
#define _C4_UTIL_HPP_

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
// CONFIG

//#define C4_USE_XASSERT
//#define C4_LOG_THREAD_SAFE
#ifndef C4_LOG_MAX_CHANNELS
#   define C4_LOG_MAX_CHANNELS 32
#endif

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
// PREPROCESSOR UTILS

#define C4_COUNTOF(arr) sizeof(arr)/sizeof((arr)[0])

#define C4_EXPAND(arg) arg
#define C4_COMMA ,

/** expand and quote */
#define C4_XQUOTE(arg) _C4_XQUOTE(arg)
#define _C4_XQUOTE(arg) C4_QUOTE(arg)
#define C4_QUOTE(arg) #arg

/** expand and concatenate */
#define C4_XCAT(arg1, arg2) _C4_XCAT(arg1, arg2)
#define _C4_XCAT(arg1, arg2) C4_CAT(arg1, arg2)
#define C4_CAT(arg1, arg2) arg1##arg2

#define C4_VERSION_CAT(major, minor, patch) ((major)*10000 + (minor)*100 + (patch))

/* NOTE: using , ## __VA_ARGS__ to deal with zero-args calls to
 * variadic macros is not portable, but works in clang, gcc, msvc, icc.
 * clang requires switching off compiler warnings for pedantic mode.
 * @see http://stackoverflow.com/questions/32047685/variadic-macro-without-arguments */
#ifdef __clang__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments" // warning: token pasting of ',' and __VA_ARGS__ is a GNU extension
#endif

/** A preprocessor foreach. Spectacular trick taken from:
 * http://stackoverflow.com/a/1872506/5875572
 * The first argument is for a macro receiving a single argument,
 * which will be called with every subsequent argument. There is
 * currently a limit of 20 arguments, and at least 1 must be provided.
 *
Example:
@code{.cpp}
#include <cstddef>
struct Example {
    int a;
    int b;
    int c;
};
// define a one-arg macro to be called
#define PRN_STRUCT_OFFSETS(field) PRN_STRUCT_OFFSETS_(Example, field)
#define PRN_STRUCT_OFFSETS_(structure, field) printf(C4_XQUOTE(structure) ":" C4_XQUOTE(field)" - offset=%zu\n", offsetof(structure, field));

// now call the macro for a, b and c
C4_FOR_EACH(PRN_STRUCT_OFFSETS, a, b, c);
@endcode */
#define C4_FOR_EACH(what, ...) C4_FOR_EACH_SEP(what, ;, __VA_ARGS__)
/** same as C4_FOR_EACH(), but use a custom separator between statements.
 * If a comma is wanted as the separator, use the C4_COMMA macro.
 * @see C4_FOR_EACH */
#define C4_FOR_EACH_SEP(what, sep, ...) C4_FOR_EACH_(C4_FOR_EACH_NARG(__VA_ARGS__), what, sep, __VA_ARGS__)
#define C4_FOR_EACH_01(what, sep, x) what(x) sep
#define C4_FOR_EACH_02(what, sep, x, ...) what(x) sep C4_FOR_EACH_01(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_03(what, sep, x, ...) what(x) sep C4_FOR_EACH_02(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_04(what, sep, x, ...) what(x) sep C4_FOR_EACH_03(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_05(what, sep, x, ...) what(x) sep C4_FOR_EACH_04(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_06(what, sep, x, ...) what(x) sep C4_FOR_EACH_05(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_07(what, sep, x, ...) what(x) sep C4_FOR_EACH_06(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_08(what, sep, x, ...) what(x) sep C4_FOR_EACH_07(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_09(what, sep, x, ...) what(x) sep C4_FOR_EACH_08(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_10(what, sep, x, ...) what(x) sep C4_FOR_EACH_09(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_11(what, sep, x, ...) what(x) sep C4_FOR_EACH_10(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_12(what, sep, x, ...) what(x) sep C4_FOR_EACH_11(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_13(what, sep, x, ...) what(x) sep C4_FOR_EACH_12(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_14(what, sep, x, ...) what(x) sep C4_FOR_EACH_13(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_15(what, sep, x, ...) what(x) sep C4_FOR_EACH_14(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_16(what, sep, x, ...) what(x) sep C4_FOR_EACH_15(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_17(what, sep, x, ...) what(x) sep C4_FOR_EACH_16(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_18(what, sep, x, ...) what(x) sep C4_FOR_EACH_17(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_19(what, sep, x, ...) what(x) sep C4_FOR_EACH_18(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_20(what, sep, x, ...) what(x) sep C4_FOR_EACH_19(what, sep, __VA_ARGS__)
#define C4_FOR_EACH_NARG(...) C4_FOR_EACH_NARG_(__VA_ARGS__, C4_FOR_EACH_RSEQ_N())
#define C4_FOR_EACH_NARG_(...) C4_FOR_EACH_ARG_N(__VA_ARGS__)
#define C4_FOR_EACH_ARG_N(_01, _02, _03, _04, _05, _06, _07, _08, _09, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, N, ...) N
#define C4_FOR_EACH_RSEQ_N() 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 09, 08, 07, 06, 05, 04, 03, 02, 01, 0
#define C4_FOR_EACH_(N, what, sep, ...) C4_XCAT(C4_FOR_EACH_, N)(what, sep, __VA_ARGS__)

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
// PLATFORM

#if defined(_WIN64)
#   define C4_WIN
#   define C4_WIN64
#elif defined(_WIN32)
#   define C4_WIN
#   define C4_WIN32
#elif defined(__ANDROID__)
#   define C4_ANDROID
#elif defined(__APPLE__)
#   include "TargetConditionals.h"
#   if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#       define C4_IOS
#   elif TARGET_OS_MAC || TARGET_OS_OSX
#       define C4_MACOS
#   else
#       error "Unknown Apple platform"
#   endif
#elif defined(__unix) || defined(__linux)
#   define C4_UNIX
#else
#   error "unknown platform"
#endif

#if defined(__posix) || defined(__unix__) || defined(__linux)
#   define C4_POSIX
#endif

#ifdef C4_WIN
#   define NOMINMAX
#   define VC_EXTRALEAN
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#endif

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
// PROCESSOR

#if defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
#   define C4_CPU_X86_64
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
#   define C4_CPU_X86
#elif defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
#   define C4_CPU_IA64
#elif defined(__arm__) || defined(__TARGET_ARCH_ARM)
#   define C4_CPU_ARM
#   if defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) \
    || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) \
    || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 7)
#       define C4_CPU_ARMV7
#   elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) \
    || defined(__ARM_ARCH_6T2__) || defined(__ARM_ARCH_6Z__) \
    || defined(__ARM_ARCH_6K__)  || defined(__ARM_ARCH_6ZK__) \
    || defined(__ARM_ARCH_6M__) \
    || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 6)
#       define C4_CPU_ARMV6
#   elif defined(__ARM_ARCH_5TEJ__) \
    || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 5)
#       define C4_CPU_ARMV5
#   else
#       error "unknown CPU architecture: ARM"
#   endif
#elif defined(__ppc__) || defined(__ppc) || defined(__powerpc__) \
    || defined(_ARCH_COM) || defined(_ARCH_PWR) || defined(_ARCH_PPC)  \
    || defined(_M_MPPC) || defined(_M_PPC)
#   if defined(__ppc64__) || defined(__powerpc64__) || defined(__64BIT__)
#       define C4_CPU_PPC64
#   else
#       define C4_CPU_PPC
#   endif
#else
#   error "unknown CPU architecture"
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Compilers:
//      C4_MSVC
//             Visual Studio 2015: MSVC++ 14
//             Visual Studio 2013: MSVC++ 13
//             Visual Studio 2013: MSVC++ 12
//             Visual Studio 2012: MSVC++ 11
//             Visual Studio 2010: MSVC++ 10
//             Visual Studio 2008: MSVC++ 09
//             Visual Studio 2005: MSVC++ 08
//      C4_CLANG
//      C4_GCC
//      C4_ICC (intel compiler)
/** @see http://sourceforge.net/p/predef/wiki/Compilers/ for a list of compiler identifier macros */
/** @see https://msdn.microsoft.com/en-us/library/b0084kay.aspx for VS2013 predefined macros */

#if defined(_MSC_VER) && (defined(C4_WIN) || defined(C4_XBOX) || defined(C4_UE4))
#   define C4_MSVC
#   define C4_MSVC_VERSION_2015 14
#   define C4_MSVC_VERSION_2013 12
#   define C4_MSVC_VERSION_2012 11
#   if _MSC_VER == 1900
#       define C4_MSVC_VERSION C4_MSVC_VERSION_2015  // visual studio 2015
#       define C4_MSVC_2015
#   elif _MSC_VER == 1800
#       error "MSVC version not supported"
#       define C4_MSVC_VERSION C4_MSVC_VERSION_2013  // visual studio 2013
#       define C4_MSVC_2013
#   elif _MSC_VER == 1700
#       error "MSVC version not supported"
#       define C4_MSVC_VERSION C4_MSVC_VERSION_2012  // visual studio 2012
#       define C4_MSVC_2012
#   elif _MSC_VER == 1600
#       error "MSVC version not supported"
#       define C4_MSVC_VERSION 10  // visual studio 2010
#       define C4_MSVC_2010
#   elif _MSC_VER == 1500
#       error "MSVC version not supported"
#       define C4_MSVC_VERSION 09  // visual studio 2008
#       define C4_MSVC_2008
#   elif _MSC_VER == 1400
#       error "MSVC version not supported"
#       define C4_MSVC_VERSION 08  // visual studio 2005
#       define C4_MSVC_2005
#   else
#       error "MSVC version not supported"
#   endif // _MSC_VER
#else
#   define C4_MSVC_VERSION 0   // visual studio not present
#   define C4_GCC_LIKE
#   ifdef __INTEL_COMPILER // check ICC before checking GCC, as ICC defines __GNUC__ too
#       define C4_ICC
#       define C4_ICC_VERSION __INTEL_COMPILER
#   elif defined(__clang__)
#       define C4_CLANG
#       ifndef __apple_build_version__
#           define C4_CLANG_VERSION C4_VERSION_ENCODED(__clang_major__, __clang_minor__, __clang_patchlevel__)
#       else
#           define C4_CLANG_VERSION __apple_build_version__
#       endif
#   elif defined(__GNUC__)
#       define C4_GCC
#       if defined(__GNUC_PATCHLEVEL__)
#           define C4_GCC_VERSION C4_VERSION_ENCODED(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#       else
#           define C4_GCC_VERSION C4_VERSION_ENCODED(__GNUC__, __GNUC_MINOR__, 0)
#       endif
#   endif
#endif // defined(C4_WIN) && defined(_MSC_VER)

//------------------------------------------------------------

#define _C4_BEGIN_NAMESPACE(ns) namespace ns {
#define _C4_END_NAMESPACE(ns) }

#define C4_BEGIN_NAMESPACE(...) C4_FOR_EACH_SEP(_C4_BEGIN_NAMESPACE, /**/,__VA_ARGS__)
#define C4_END_NAMESPACE(...) C4_FOR_EACH_SEP(_C4_END_NAMESPACE, /**/, __VA_ARGS__)

//------------------------------------------------------------

#ifndef C4_API
#   if defined(_MSC_VER)
#       if defined(C4_EXPORT)
#           define C4_API __declspec(dllexport)
#       elif defined(C4_IMPORT)
#           define C4_API __declspec(dllimport)
#       else
#           define C4_API
#       endif
#   else
#       define C4_API
#   endif
#endif

#ifndef _MSC_VER  /// @todo assuming gcc-like compiler. check it is actually so.
/** for function attributes in GCC,
 * @see https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes */
/** for __builtin functions in GCC,
 * @see https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html */
#   define C4_RESTRICT __restrict__
#   define C4_RESTRICT_FN __attribute__((restrict))
#   define C4_NO_INLINE __attribute__((noinline))
#   define C4_ALWAYS_INLINE inline __attribute__((always_inline))
/** force inlining of every callee function */
#   define C4_FLATTEN __atribute__((flatten))
/** mark a function as hot, ie as having a visible impact in CPU time
 * thus making it more likely to inline, etc
 * @see http://stackoverflow.com/questions/15028990/semantics-of-gcc-hot-attribute */
#   define C4_HOT __attribute__((hot))
/** mark a function as cold, ie as NOT having a visible impact in CPU time
 * @see http://stackoverflow.com/questions/15028990/semantics-of-gcc-hot-attribute */
#   define C4_COLD __attribute__((cold))
/** @see https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html */
#   define C4_BUILTIN_EXPECT(x, y) __builtin_expect(x, y)
#   define C4_LIKELY(x)   __builtin_expect(x, 1)
#   define C4_UNLIKELY(x) __builtin_expect(x, 0)

#else
#   define C4_RESTRICT __restrict
#   define C4_RESTRICT_FN __declspec(restrict)
#   define C4_NO_INLINE __declspec(noinline)
#   define C4_ALWAYS_INLINE inline __forceinline
/** this is not available in VS AFAIK */
#   define C4_FLATTEN
#   define C4_HOT         /** @todo */
#   define C4_COLD        /** @todo */
#   define C4_BUILTIN_EXPECT(x, y) x /** @todo */
#   define C4_LIKELY(x)   x /** @todo */
#   define C4_UNLIKELY(x) x /** @todo */
#endif

#ifndef _MSC_VER
#   define C4_FUNC __FUNCTION__
#   define C4_PRETTY_FUNC __PRETTY_FUNCTION__
#else /// @todo assuming gcc-like compiler. check it is actually so.
#   define C4_FUNC __FUNCTION__
#   define C4_PRETTY_FUNC __FUNCSIG__
#endif

/** prevent an empty loop from being optimized out.
 * @see http://stackoverflow.com/a/7084193/5875572 */
#define C4_KEEP_EMPTY_LOOP asm("")

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
// error reporting

#include <cstdlib>

#define C4_ERROR(msg, ...)                                              \
    C4_LOG("\n%s:%d: ERROR: " msg "\nABORTING...\n", __FILE__, __LINE__, ## __VA_ARGS__); \
    abort();
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
#   define C4_XASSERT(cond) C4_CHECK(cond)
#   define C4_XASSERT_MSG(cond, fmt, ...) C4_CHECK_MSG(cond, fmt, ## __VA_ARGS__)
#else
#   define C4_XASSERT(cond)
#   define C4_XASSERT_MSG(cond, fmt, ...)
#endif

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
// TIMING

#if !defined(C4_WIN) && !defined(C4_POSIX)
#   include <chrono>
#endif

C4_BEGIN_NAMESPACE(c4)

/** a general-use time stamp in microseconds (usecs).
 * Although this is timed precisely, there may be some issues.
 * Eg, concurrent or heavy use may cause penalties.
 * @see https://www.strchr.com/performance_measurements_with_rdtsc
 * @see https://msdn.microsoft.com/en-us/library/windows/desktop/ee417693(v=vs.85).aspx */
inline double currtime()
{
#ifdef C4_WIN
    static bool gotfreq = false;
    static double ifreq = 0.;
    if(C4_UNLIKELY(!gotfreq))
    {
        static LARGE_INTEGER freq = {};
        QueryPerformanceFrequency(&freq);
        ifreq = 1.e9 / double(freq.QuadPart);
    }
    LARGE_INTEGER ts;
    QueryPerformanceCounter(&ts);
    double usecs = ts.QuadPart * ifreq;
    return usecs;
#elif defined(C4_POSIX)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    double usecs = 1.e6 * double(ts.tv_sec) + 1.e-3 * double(ts.tv_nsec);
    return usecs;
#else
    using hrc_type = std::chrono::high_resolution_clock;
    using usec_type = std::chrono::nanoseconds;
    using tp_type = std::chrono::time_point< hrc_type, usec_type >;
    tp_type tp = hrc_type::now();
    double nsecs = tp.time_since_epoch().count();
    return 1.e-3 * nsecs;
#endif
}

/** execution time */
inline double exetime()
{
    static const double atstart = currtime();
    double now = currtime() - atstart;
    return now;
}

/** do a spin loop for at least the given time */
inline void busy_wait(double microsecs)
{
    double start = currtime();
    while(currtime() - start < microsecs)
    {
        C4_KEEP_EMPTY_LOOP;
    }
}

C4_END_NAMESPACE(c4)

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
// LOGGING

#include <vector>
#ifdef C4_LOG_THREAD_SAFE
#   include <map>
#   include <thread>
#   include <mutex>
#endif

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <climits>
#include <cassert>

C4_BEGIN_NAMESPACE(c4)

// the buffer used for string conversion
struct LogBuffer
{
    std::vector< char > buf;
    size_t pos;

    LogBuffer() : buf(512), pos(0) {}

    // string to read from
    C4_ALWAYS_INLINE const char* rd() const { return &buf.front(); }
    // string to write into
    C4_ALWAYS_INLINE char* wt() { return &buf.front() + pos; }
    // remaining size
    C4_ALWAYS_INLINE size_t rem() { return buf.size() - pos; }

    void clear()
    {
        pos = 0;
        buf[0] = '\0';
    }
    void growto(size_t sz) // grow by the max of sz and the golden ratio
    {
        size_t next = size_t(1.618f * float(buf.size()));
        next = next > sz ? next : sz;
        buf.resize(next);
    }
    void write(const char *cstr)
    {
        write(cstr, strlen(cstr));
    }
    void write(const char *str, size_t sz)
    {
        if(sz+1 > rem()) growto(pos + sz + 1);
        strncpy(wt(), str, sz);
        pos += sz;
        buf[pos] = '\0';
    }
    void printf(const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        int num = vsnprintf(wt(), rem(), fmt, args);
        if(num >= (int)rem()) // not enough space?
        {
            growto(buf.size() + num + 1);
            num = vsnprintf(wt(), rem(), fmt, args);
        }
        pos += num;
        buf[pos] = '\0';
    }
    void vprintf(const char *fmt, va_list args)
    {
        /* WATCHOUT. reusing va_list is not portable. That works in Windows
         * but not in Linux. Note that passing a va_list argument to a
         * function IS reuse. So the copy needs to be made here because
         * we _may_ need to use it again.
         * @see http://www.bailopan.net/blog/?p=30 */
        va_list args2;
        va_copy(args2, args);
        int num = vsnprintf(wt(), rem(), fmt, args);
        if(num >= (int)rem()) // not enough space?
        {
            growto(buf.size() + num + 1);
            num = vsnprintf(wt(), rem(), fmt, args2); // <------------ WATCHOUT!!!
        }
        pos += num;
        buf[pos] = '\0';
    }
};

//-----------------------------------------------------------------------------
class Log
{
public:

    typedef enum {
        ALWAYS = -10,
        ERR = -2,
        WARN = -1,
        INFO = 0,
        DEBUG = 1,
        TRACE1 = 2,
        TRACE2 = 3,
        TRACE3 = 4,
    } Level_e;

    typedef enum {
        SHOW_TIMESTAMP = 1 << 0,
        TO_TERM = 1 << 1,
        TO_FILE = 1 << 2,
        TO_STR  = 1 << 3,
        DEFAULT_MODE = SHOW_TIMESTAMP|TO_TERM,
    } Mode_e;

    struct Channel
    {
        uint8_t level;
        uint8_t name_len;
        char    name[30];

        Channel() : level{INFO}, name_len{0}, name{0} {}
        void init(const char *str, Level_e lev)
        {
            level = lev;
            name_len = strlen(str);
            if(name_len > (sizeof(name) - 1)) abort();
            if(name_len > 0)
            {
                snprintf(name, sizeof(name), "%s", str);
            }
            else
            {
                name[name_len] = '\0';
            }
        }
        inline bool cmp(const char *str) const
        {
            if(name_len == 0) return str[0] == '\0';
            return strncmp(name, str, name_len) == 0;
        }
        inline bool skip(Level_e lev) const
        {
            return lev < level || lev == ALWAYS;
        }
    };

    static uint8_t& mode() { static uint8_t m = DEFAULT_MODE; return m; }
    static void mode(uint8_t mode_flags) { mode() = mode_flags; }

    static FILE*& file() { static FILE *f = nullptr; return f; }
    static void file(FILE* f) { file() = f; }

    static LogBuffer& strbuf() { static LogBuffer b; return b; }
#ifdef C4_LOG_THREAD_SAFE
    static std::mutex& strbuf_mtx() { static std::mutex m; return m; }
#endif

    /** return the buffer for this thread */
    static LogBuffer& buf()
    {
        // using static vars here saves us the need to declare them
        // in a source file, allowing to use this as a drop-in header.
#ifndef C4_LOG_THREAD_SAFE
        static LogBuffer b;
        return b;
#else // C4_LOG_THREAD_SAFE
        // using a map saves us from locking when formatting the buffer
        static std::map< std::thread::id, LogBuffer > s_buffers;
        static std::mutex s_buffers_mtx;
        auto id = std::this_thread::get_id();
        LogBuffer *b = nullptr;
        {
            std::lock_guard< std::mutex > lock(s_buffers_mtx);
            b = &s_buffers[id];
        }
        return *b;
#endif // C4_LOG_THREAD_SAFE
    }

    static inline std::vector< Channel >& _channels()
    {
        static std::vector< Channel > s_channels;
        if(s_channels.capacity() == 0)
        {
            s_channels.reserve(C4_LOG_MAX_CHANNELS);
            s_channels.resize(1);
        }
        return s_channels;
    }
    static Channel* main_channel()
    {
        return &_channels()[0];
    }
    static Channel* channel(uint8_t i)
    {
        assert(i < _channels().size());
        return &_channels()[i];
    }
    static Channel* channel(const char *name)
    {
        for(auto &ch : _channels())
        {
            if(ch.cmp(name))
            {
                return &ch;
            }
        }
        return nullptr;
    }
    static Channel* add_channel(const char *name, Level_e lev = INFO)
    {
        auto& chs = _channels();
        assert(chs.size() <= C4_LOG_MAX_CHANNELS);
        assert(channel(name) == nullptr);
        chs.emplace_back();
        Channel& ch = chs.back();
        ch.init(name, lev);
        return &ch;
    }
    /** set the level of all channels */
    static void level(Level_e l)
    {
        for(auto &ch : _channels())
        {
            ch.level = l;
        }
    }

    static void _print_prefix(Channel const& ch, LogBuffer &buf)
    {
        uint8_t md = mode();
        if((md & SHOW_TIMESTAMP) && (ch.name_len > 0))
        {
            buf.printf("%lfms[%s]: ", exetime()/1.e3, ch.name);
        }
        else if((md & SHOW_TIMESTAMP))
        {
            buf.printf("%lfms: ", exetime()/1.e3, ch.name);
        }
        else if((ch.name_len > 0))
        {
            buf.printf("[%s]: ", ch.name);
        }
    }

    /** print formatted output to the main channel, at INFO level */
    static void printf(const char *fmt, ...)
    {
        Channel &ch = *main_channel();
        if(ch.skip(INFO)) return;
        va_list args;
        va_start(args, fmt);
        auto& b = buf();
        _print_prefix(ch, b);
        b.vprintf(fmt, args);
        pump(b.rd(), b.pos);
        b.clear();
    }
    /** print formatted output to the main channel, at the given level */
    static void printfl(Level_e level, const char *fmt, ...)
    {
        Channel &ch = *main_channel();
        if(ch.skip(level)) return;
        va_list args;
        va_start(args, fmt);
        auto& b = buf();
        _print_prefix(ch, b);
        b.vprintf(fmt, args);
        pump(b.rd(), b.pos);
        b.clear();
    }
    /** print formatted output to the given channel at the given level */
    static void printfcl(Channel *ch, Level_e level, const char *fmt, ...)
    {
        if(ch->skip(level)) return;
        va_list args;
        va_start(args, fmt);
        auto& b = buf();
        _print_prefix(*ch, b);
        b.vprintf(fmt, args);
        pump(b.rd(), b.pos);
        b.clear();
    }

    /** directly print a string to the main channel at INFO level */
    static void write(const char *s) { write(s, strlen(s)); }
    /** directly print a string with specified size to the main channel at INFO level */
    static void write(const char *s, size_t sz)
    {
        Channel &ch = *main_channel();
        if(ch.skip(INFO)) return;
        auto& b = buf();
        _print_prefix(ch, b);
        b.write(s, sz);
        pump(b.rd(), sz);
        b.clear();
    }
    /** directly print a string to the main channel at the given level */
    static void writel(Level_e level, const char *s) { writel(level, s, strlen(s)); }
    /** directly print a string with specified size to the main channel at the given level */
    static void writel(Level_e level, const char *s, size_t sz)
    {
        Channel &ch = *main_channel();
        if(ch.skip(level)) return;
        auto& b = buf();
        _print_prefix(ch, b);
        b.write(s, sz);
        pump(b.rd(), sz);
        b.clear();
    }
    /** directly print a string to the given channel at the given level */
    static void writecl(Channel *ch, Level_e level, const char *s) { writel(level, s, strlen(s)); }
    /** directly print a string with specified size to the given channel at the given level */
    static void writecl(Channel *ch, Level_e level, const char *s, size_t sz)
    {
        if(ch->skip(level)) return;
        auto& b = buf();
        _print_prefix(*ch, b);
        b.write(s, sz);
        pump(b.rd(), sz);
        b.clear();
    }

    static void pump(const char *str, size_t sz)
    {
        uint8_t md = mode();
        if(md & TO_TERM)
        {
#ifndef _MSC_VER
            ::printf("%.*s", (int)sz, str);
#else
            if( ! IsDebuggerPresent())
            {
                ::printf("%.*s", (int)sz, str);
            }
            else
            {
                OutputDebugStrA(str);
            }
#endif
        }
        if(md & TO_FILE)
        {
            if(file() == nullptr) abort();
            fprintf(file(), "%.*s", (int)sz, str);
        }
        if(md & TO_STR)
        {
#ifdef C4_LOG_THREAD_SAFE
            std::lock_guard< std::mutex > lock(strbuf_mtx());
#endif
            strbuf().write(str, sz);
        }
    }

    static void flush()
    {
        uint8_t md = mode();
        if(md & TO_TERM)
        {
#ifndef _MSC_VER
            fflush(stdout);
#else
            if( ! IsDebuggerPresent())
            {
                fflush(stdout);
            }
#endif
        }
        if(md & TO_FILE)
        {
            fflush(file());
        }
        if(md & TO_STR)
        {
#ifdef C4_LOG_THREAD_SAFE
            std::lock_guard< std::mutex > lock(strbuf_mtx());
#endif
            strbuf().clear();
        }
    }

    /** A proxy object which buffers prints to a log buffer.
     * It accumulates << calls and outputs once after the last call.
     * The buffer is set to NULL when the channel's log level
     * is incompatible with the given log level. */
    struct Proxy
    {
        Channel const& channel;
        Level_e level;
        LogBuffer* buf;
        Proxy(Channel const* ch, Level_e lev) : channel(*ch), level(lev), buf(nullptr)
        {
            if(C4_LIKELY(channel.skip(level))) return;
            buf = &Log::buf();
            Log::_print_prefix(channel, *buf);
        }
        ~Proxy()
        {
            if(C4_LIKELY(!buf)) return;
            Log::pump(buf->rd(), buf->pos);
            buf->clear();
        }
        template< typename T >
        C4_ALWAYS_INLINE void printf(const char *fmt, T const& var) const
        {
            if(C4_LIKELY(!buf)) return;
            buf->printf(fmt, var);
        }
    };
    Proxy operator() (Channel const *ch, Level_e lev) { return Proxy(ch, lev); }
    Proxy operator() (Channel const *ch) { return Proxy(ch, INFO); }
    Proxy operator() (Level_e lev) { return Proxy(&_channels()[0], INFO); }

    /** create a temporary proxy object to handle all the calls to <<.
     * It will accumulate the calls and output once after the last call. */
    template< class T >
    inline Proxy operator<< (T const& v)
    {
        Proxy s(&_channels()[0], INFO);
        s << v;
        return s;
    }
};

typedef const Log::Proxy LogProxy;

inline LogProxy& operator<< (LogProxy& ss, void *      var) { ss.printf("%p",   var); return ss; }
inline LogProxy& operator<< (LogProxy& ss, double      var) { ss.printf("%lg",  var); return ss; }
inline LogProxy& operator<< (LogProxy& ss, float       var) { ss.printf("%g",   var); return ss; }
inline LogProxy& operator<< (LogProxy& ss,  int64_t    var) { ss.printf("%lld", var); return ss; }
inline LogProxy& operator<< (LogProxy& ss, uint64_t    var) { ss.printf("%llu", var); return ss; }
inline LogProxy& operator<< (LogProxy& ss,  int32_t    var) { ss.printf("%d",   var); return ss; }
inline LogProxy& operator<< (LogProxy& ss, uint32_t    var) { ss.printf("%u",   var); return ss; }
inline LogProxy& operator<< (LogProxy& ss,  int16_t    var) { ss.printf("%hd",  var); return ss; }
inline LogProxy& operator<< (LogProxy& ss, uint16_t    var) { ss.printf("%hu",  var); return ss; }
inline LogProxy& operator<< (LogProxy& ss,  int8_t     var) { ss.printf("%hhd", var); return ss; }
inline LogProxy& operator<< (LogProxy& ss, uint8_t     var) { ss.printf("%hhu", var); return ss; }
inline LogProxy& operator<< (LogProxy& ss,       char *var) { ss.printf("%s",   var); return ss; }
inline LogProxy& operator<< (LogProxy& ss, const char *var) { ss.printf("%s",   var); return ss; }
//inline LogProxy& operator<< (LogProxy& ss, std::string const& var) { ss.printf(var.c_str(), var.size()); return ss; }
template< size_t N >
inline LogProxy& operator<< (LogProxy& ss, const char (&var)[N]) { ss.printf(&var[0], N-1); return ss; }

#define c4_log c4::Log()
#define C4_LOG(fmt, ...) c4_log.printf(fmt, ## __VA_ARGS__)

C4_END_NAMESPACE(c4)

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif //_C4_UTIL_HPP_
