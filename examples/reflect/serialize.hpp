#ifndef _C4_SERIALIZE_HPP_
#define _C4_SERIALIZE_HPP_

#include <type_traits>

namespace c4 {

// forward declarations
template< class Stream >
class Archive;

template< class T, class Stream >
inline void serialize(Archive< Stream > &a, const char* name, T *var);
template< class T, class Stream >
inline void serialize(Archive< Stream > &a, const char* name, T *var, size_t num);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** @see serialize_category<>
 *------------------------------------------------------------------------------
 *    to serialize:                     N objects
 *--------------------------------------------+---------------------------------
 *      in format: |         binary           |       text(yml,xml,json)
 *-----------------+--------------------------+---------------------------------
 * NATIVE          |      1 * memcpy()        |  N * (operator>> or operator<<)
 *-----------------+--------------------------+---------------------------------
 * CUSTOM          |   N * (c4::serialize())  |     N * (c4::serialize())
 *-----------------+--------------------------+---------------------------------
 * CUSTOM_TXT      |      1 * memcpy()        |     N * (c4::serialize())
 *-----------------+--------------------------+---------------------------------
 */
enum class SerializeCategory_e : int
{
    /** for fundamental types, pod types, and C-style arrays thereof.
     * eg: int, int[], int[N], float, double, pod structs, etc */
    NATIVE = 0,
    /** for classes that provide specializations of c4::serialize(). */
    CUSTOM,
    /** mix between NATIVE and CUSTOM: memcpy() for binary archives,
     * but uses specialization of c4::serialize() for text archives
     * @todo implement this*/
    // CUSTOM_TXT,

    //inline operator int () const { return static_cast< int >(*this); }
    //inline bool operator== (int v) const { return static_cast< SerializeCategory_e >(v) == *this; }
};

template< class T >
struct serialize_category
{
    enum : int { value = (int)SerializeCategory_e::NATIVE };
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

    typedef enum {
        READ = 0,
        WRITE = 1,
    } ArchiveMode_e;

public:

    bool is_reading() const { return m_mode == READ; }
    bool is_writing() const { return m_mode == WRITE; }

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
        serialize(*this, name, var);
        pop(name);
    }

    template< class T >
    _c4sfinae(void, NATIVE) operator()(const char* name, T *var, size_t num)
    {
        push_seq(name, num);
        m_stream(var, num);
        pop_seq(name);
    }
    template< class T >
    _c4sfinae(void, CUSTOM) operator()(const char* name, T *var, size_t num)
    {
        push_seq(name, num);
        for(size_t i = 0; i < num; ++i)
        {
            serialize(*this, name, var + i);
        }
        pop_seq(name, num);
    }

    void push(const char* name) { m_stream.push_var(name); }
    void pop(const char* name) { m_stream.pop_var(name); }
    void push_seq(const char* name, size_t num) { m_stream.push_seq(name, num); }
    void pop_seq(const char* name, size_t num) { m_stream.pop_seq(name, num); }

private:

    ArchiveMode_e m_mode;

    Stream m_stream;

};

#undef _c4sfinae

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** utility struct to allow specialization. When specializing, the
 * serialize_category<>::value must be set to custom.  */
template< class T, class Stream >
struct srlz_custom
{
    static void s(Archive< Stream > &a, const char* name, T *var)
    {
        a(name, var);
    }
    static void s(Archive< Stream > &a, const char* name, T *var, size_t num)
    {
        a(name, var, num);
    }
};

template< class T, class Stream >
void serialize(Archive< Stream > &a, const char* name, T *var)
{
    srlz_custom< T, Stream >::s(a, name, var);
}
template< class T, class Stream >
void serialize(Archive< Stream > &a, const char* name, T *var, size_t num)
{
    srlz_custom< T, Stream >::s(a, name, var, num);
}

} // end namespace c4

#endif // _C4_SERIALIZE_HPP_
