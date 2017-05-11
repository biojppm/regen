#ifndef _C4_ENUM_HPP_
#define _C4_ENUM_HPP_

#include <cstring>
#include <cassert>

#define C4_ENUM(...)

template< class T >
struct EnumAndName
{
    T value;
    const char *name;
};

/** a container for all enum value-name pairs */
template< class T >
class EnumPairs
{
public:

    EnumPairs(EnumAndName< T > const *p, size_t n)
     : m_pairs(p), m_num(n)
    {}

    template< size_t N >
    EnumPairs(EnumAndName< T > const (&p)[N])
     : m_pairs(p), m_num(N)
    {}

    EnumAndName< T > const* find(T v) const;
    EnumAndName< T > const* find(const char *s) const;
    EnumAndName< T > const* find(const char *s, size_t len) const;

    EnumAndName< T > const* get(T v) const { auto p = find(v); assert(p != nullptr); return p; }
    EnumAndName< T > const* get(const char *s) const { auto p = find(s); assert(p != nullptr); return p; }
    EnumAndName< T > const* get(const char *s, size_t len) const { auto p = find(s, len); assert(p != nullptr); return p; }

    EnumAndName< T > const* begin() const { return m_pairs; }
    EnumAndName< T > const* end  () const { return m_pairs + m_num; }

    size_t size() const { return m_num; }

    EnumAndName< T > const& operator[] (size_t i) const
    { assert(i < m_num); return *(m_pairs + i); }

private:

    EnumAndName< T > const *m_pairs;
    size_t m_num;

};

/** This function needs to be specialized for each enum type.
 * Failing to provide a specialization will cause a link-time error. */
template< class T >
const EnumPairs< T > enum_pairs();

/** get the string corresponding to an enum value */
template< class T >
const char* e2str(T e)
{
    auto pairs = enum_pairs< T >();
    auto *p = pairs.get(e);
    return p->name;
}
/** get the enum value corresponding to a string */
template< class T >
T str2e(const char* str)
{
    auto pairs = enum_pairs< T >();
    auto *p = pairs.get(str);
    return p->value;
}


//-----------------------------------------------------------------------------
/** returns nullptr when none is found */
template< class T > EnumAndName< T > const* EnumPairs< T >::find(T v) const
{
    for(auto *p = m_pairs, *e = p+m_num; p < e; ++p)
        if(p->value == v)
            return p;
    return nullptr;
}
/** returns nullptr when none is found */
template< class T > EnumAndName< T > const* EnumPairs< T >::find(const char *s) const
{
    for(auto *p = m_pairs, *e = p+m_num; p < e; ++p)
        if(strcmp(p->name, s) == 0)
            return p;
    return nullptr;
}
/** returns nullptr when none is found */
template< class T > EnumAndName< T > const* EnumPairs< T >::find(const char *s, size_t len) const
{
    for(auto *p = m_pairs, *e = p+m_num; p < e; ++p)
        if(strncmp(p->name, s, len) == 0)
            return p;
    return nullptr;
}

#endif // _C4_ENUM_HPP_
