#ifndef _C4_SERIALIZE_HPP_
#define _C4_SERIALIZE_HPP_

#include <type_traits>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "util.hpp"


namespace c4 {

// forward declarations
template< class Stream > class Archive;
template< class T, class Stream > void serialize(Archive< Stream > &a, const char* name, T *var);
template< class T, class Stream > void serialize(Archive< Stream > &a, const char* name, T *var, size_t num);


//-----------------------------------------------------------------------------

#define C4_DECLARE_SERIALIZE_METHOD()                               \
public:                                                             \
    template< class Stream >                                        \
    void serialize(c4::Archive< Stream > &a, const char* name);     \
private:

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** @see serialize_category<>
 *-----------------------------------------------------------------------------
 *    to serialize:                     N objects
 *-------------------------------------------+---------------------------------
 *      in format: |        binary           |       text(yml,xml,json)
 *-----------------+-------------------------+---------------------------------
 * NATIVE          |     1 * memcpy()        |  N * (operator>> or operator<<)
 *-----------------+-------------------------+---------------------------------
 * CUSTOM          |  N * (c4::serialize())  |     N * (c4::serialize())
 *-----------------+-------------------------+---------------------------------
 * METHOD          |  N * (var->serialize()) |     N * (var->serialize())
 *-----------------+-------------------------+---------------------------------
 */
enum class SerializeCategory_e : int
{
    /** for fundamental types, pod types, and C-style arrays thereof.
     * eg: int, int[], int[N], float, double, pod structs, etc */
    NATIVE = 0,
    /** for classes that provide specializations of c4::serialize(). */
    CUSTOM,
    /** for classes that provide a member serialize() method. */
    METHOD,
    /** mix between NATIVE and CUSTOM: memcpy() for binary archives,
     * but uses specialization of c4::serialize() for text archives
     * @todo implement this*/
    // CUSTOM_TXT,

    //inline bool operator== (int v) const { return static_cast< SerializeCategory_e >(v) == *this; }
};

template< class T >
struct serialize_category
{
    enum : int { value = static_cast< int >(SerializeCategory_e::NATIVE) };
};
/** makes C-style arrays use their type without extent */
template< class T >
struct serialize_category< T[] >
{
    enum : int { value = serialize_category< T >::value };
};
/** makes C-style arrays use their type without extent */
template< class T, size_t N >
struct serialize_category< T[N] >
{
    enum : int { value = serialize_category< T >::value };
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define _c4sfinae(type_, category_) \
    typename std::enable_if                                             \
    <                                                                   \
        serialize_category< T >::value                                  \
        ==                                                              \
        (int)SerializeCategory_e::category_                             \
        ,                                                               \
        type_                                                           \
    >::type

template< class Stream >
class Archive
{
public:

    template< class... Args >
    void write_mode(bool yes, Args&&... args)
    {
        m_stream.write_mode(yes, std::forward< Args >(args)...);
    }
    bool write_mode() const { return m_stream.write_mode(); }

public:

    template< class T >
    _c4sfinae(void, NATIVE) operator()(const char* name, T *var)
    {
        push(name);
        m_stream(var);
        pop(name);
    }
    template< class T >
    _c4sfinae(void, CUSTOM) operator()(const char* name, T *var)
    {
        push(name);
        c4::serialize(*this, name, var);
        pop(name);
    }
    template< class T >
    _c4sfinae(void, METHOD) operator()(const char* name, T *var)
    {
        push(name);
        var->serialize(*this, name);
        pop(name);
    }

    template< class T, size_t N >
    void operator()(const char *name, T (*var)[N])
    {
        (*this)(name, *var, N);
    }

    template< class T >
    _c4sfinae(void, NATIVE) operator()(const char* name, T *var, size_t num)
    {
        push_seq(name, num);
        m_stream(var, num);
        pop_seq(name, num);
    }
    template< class T >
    _c4sfinae(void, CUSTOM) operator()(const char* name, T *var, size_t num)
    {
        push_seq(name, num);
        for(size_t i = 0; i < num; ++i)
        {
            c4::serialize(*this, name, var + i);
        }
        pop_seq(name, num);
    }
    template< class T >
    _c4sfinae(void, METHOD) operator()(const char* name, T *var, size_t num)
    {
        push_seq(name, num);
        for(size_t i = 0; i < num; ++i)
        {
            (var + i)->serialize(*this, name);
        }
        pop_seq(name, num);
    }

    void push(const char* name) { m_stream.push_var(name); }
    void pop(const char* name) { m_stream.pop_var(name); }
    void push_seq(const char* name, size_t num) { m_stream.push_seq(name, num); }
    void pop_seq(const char* name, size_t num) { m_stream.pop_seq(name, num); }

private:

    Stream m_stream;

};

#undef _c4sfinae

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** utility function to allow specialization. When specializing, the
 * serialize_category<>::value must be set to custom.  */
template< class T, class Stream >
void serialize(Archive< Stream > &a, const char* name, T *var)
{
    a(name, var);
}
template< class T, class Stream >
void serialize(Archive< Stream > &a, const char* name, T *var, size_t num)
{
    a(name, var, num);
}
template< class T, size_t N, class Stream >
void serialize(Archive< Stream > &a, const char* name, T (*var)[N])
{
    a(name, var, N);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct ArchiveStreamText
{

    bool writing = true;
    int level = 0;
    FILE* file = nullptr;
#ifdef C4_DEBUG
    std::vector< char > name_check;
#endif

    bool write_mode() const { return writing; }
    void write_mode(bool yes, FILE *which = nullptr)
    {
        if(yes)
        {
            writing = true;
            file = which ? which : stdout;
        }
        else
        {
            writing = false;
            file = which ? which : stdin;
        }
    }

    void push_var(const char *name)
    {
        if(writing)
        {
            _indentw();
            fprintf(file, "%s ", name);
        }
        else
        {
            _indentr();
            int len;
            fscanf(file, "%*s%n ", &len);
            C4_CHECK(len == (int)strlen(name));
#ifdef C4_DEBUG
            name_check.resize(len + 1);
            fscanf(file, "%s", &name_check.data());
            C4_CHECK_MSG(strcmp(name_check.data(), name) == 0,
                         "expected %s got %s", name, name_check.data());
#endif
        }
        ++level;
    }

    template< class T >
    void operator() (T *var)
    {
        if(writing)
        {
            fprintc(file, *var);
        }
        else
        {
            int ret = fscanc(file, var);
            C4_CHECK(ret == 1);
        }
    }

    void pop_var(const char *name)
    {
        if(writing)
        {
            fprintf(file, "\n");
        }
        else
        {
            fscanf(file, "\n");
        }
        --level;
    }

    void push_seq(const char *name, size_t num)
    {
        push_var(name);
        if(writing)
        {
            fprintf(file, "{%zu [\n", num);
        }
        else
        {
            int ret;
            size_t check;
            ret = fscanf(file, "{%zu [\n", &check);
            C4_CHECK(ret == 1);
            C4_CHECK(check == num);
        }
    }

    template< class T >
    void operator() (T *var, size_t num)
    {
        if(writing)
        {
            for(size_t i = 0; i < num; ++i)
            {
                _indentw();
                fprintc(file, *(var + i));
                fprintf(file, "\n");
            }
        }
        else
        {
            for(size_t i = 0; i < num; ++i)
            {
                _indentr();
                int ret;
                ret = fscanc(file, (var + i));
                C4_CHECK(ret == 1);
                ret = fscanf(file, "\n");
            }
        }
    }

    void pop_seq(const char *name, size_t num)
    {
        if(writing)
        {
            fprintf(file, "]}");
        }
        else
        {
            fscanf(file, "]}");
        }
        pop_var(name);
    }

private:

    void _indentw()
    {
        C4_ASSERT(writing);
        for(int i = 0; i < level; ++i)
            fprintf(file, "  ");
    }
    void _indentr()
    {
        C4_ASSERT(!writing);
        for(int i = 0; i < level; ++i)
            fscanf(file, "  ");
    }

};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct ArchiveStreamBinary
{

    bool writing = true;
    FILE* file = nullptr;
#ifdef C4_DEBUG
    std::vector< char > name_check;
#endif

    bool write_mode() const { return writing; }
    void write_mode(bool yes, FILE *which = nullptr)
    {
        if(yes)
        {
            writing = true;
            file = which ? which : stdout;
        }
        else
        {
            writing = false;
            file = which ? which : stdin;
        }
    }

    void push_var(const char *name)
    {
    }

    template< class T >
    void operator() (T *var)
    {
        size_t numb = sizeof(T);
        if(writing)
        {
            fwrite(var, 1, numb, file);
        }
        else
        {
            size_t ret = fread(var, 1, numb, file);
            C4_CHECK(ret == numb);
        }
    }

    void pop_var(const char *name)
    {
    }

    void push_seq(const char *name, size_t num)
    {
    }

    template< class T >
    void operator() (T *var, size_t num)
    {
        size_t numb = num * sizeof(T);
        if(writing)
        {
            fwrite(var, 1, numb, file);
        }
        else
        {
            size_t ret = fread(var, 1, numb, file);
            C4_CHECK(ret == numb);
        }
    }

    void pop_seq(const char *name, size_t num)
    {
    }

};

} // end namespace c4

#endif // _C4_SERIALIZE_HPP_
