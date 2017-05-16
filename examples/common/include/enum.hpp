#ifndef _C4_ENUM_HPP_
#define _C4_ENUM_HPP_

#include "util.hpp"
#include <cstring>

//-----------------------------------------------------------------------------
/** A simple (proxy) container for the value-name pairs of an enum type.
 * Uses linear search for finds; this could be improved for time-critical
 * code. */
template< class T >
class EnumSymbols
{
public:

    struct Sym
    {
        T value;
        const char *name;

        bool cmp(const char *s) const;
        bool cmp(const char *s, size_t len) const;
    };

public:

    template< size_t N >
    EnumSymbols(Sym const (&p)[N]) : m_symbols(p), m_num(N) {}

    size_t size() const { return m_num; }

    Sym const* begin() const { return m_symbols; }
    Sym const* end  () const { return m_symbols + size(); }

    Sym const* get(T v) const { auto p = find(v); C4_CHECK_MSG(p != nullptr, "could not find symbol=%zd", (std::ptrdiff_t)v); return p; }
    Sym const* get(const char *s) const { auto p = find(s); C4_CHECK_MSG(p != nullptr, "could not find symbol \"%s\"", s); return p; }
    Sym const* get(const char *s, size_t len) const { auto p = find(s, len); C4_CHECK_MSG(p != nullptr, "could not find symbol \"%.*s\"", len, s); return p; }

    Sym const* find(T v) const;
    Sym const* find(const char *s) const;
    Sym const* find(const char *s, size_t len) const;

    Sym const& operator[] (size_t i) { C4_CHECK(i < m_num); return m_symbols[i]; }

private:

    Sym const* m_symbols;
    size_t const m_num;

};

//-----------------------------------------------------------------------------
/** return an EnumSymbols object for the enum type T
 *
 * @warning SPECIALIZE! This needs to be specialized for each enum type.
 * Failure to provide a specialization will cause a linker error. */
template< class T >
EnumSymbols< T > const esyms();

/** return the offset at which the enum symbol starts. For example,
 * eoffs< MyEnumClass >() would be 13=strlen("MyEnumClass::")
 *
 * @warning SPECIALIZE! This needs to be specialized for each C++11 enum
 * class type. */
template< class T >
size_t eoffs()
{
    return 0;
}

//-----------------------------------------------------------------------------
/** get the enum value corresponding to a c-string */
template< class T >
T str2e(const char* str)
{
    auto pairs = esyms< T >();
    auto *p = pairs.get(str);
    return p->value;
}

/** get the c-string corresponding to an enum value */
template< class T >
const char* e2str(T e)
{
    auto es = esyms< T >();
    auto *p = es.get(e);
    return p->name;
}

/** like e2str(), but skip the type for C++11 enum classes. */
template< class T >
const char* e2stroffs(T e)
{
    const char *s = e2str< T >(e) + eoffs< T >();
    return s;
}

//-----------------------------------------------------------------------------
/** returns nullptr when none is found */
template< class T >
typename EnumSymbols< T >::Sym const* EnumSymbols< T >::find(T v) const
{
    for(Sym const* p = this->m_symbols, *e = p+this->m_num; p < e; ++p)
        if(p->value == v)
            return p;
    return nullptr;
}
/** returns nullptr when none is found */
template< class T >
typename EnumSymbols< T >::Sym const* EnumSymbols< T >::find(const char *s) const
{
    for(Sym const* p = this->m_symbols, *e = p+this->m_num; p < e; ++p)
        if(p->cmp(s))
            return p;
    return nullptr;
}
/** returns nullptr when none is found */
template< class T >
typename EnumSymbols< T >::Sym const* EnumSymbols< T >::find(const char *s, size_t len) const
{
    for(Sym const* p = this->m_symbols, *e = p+this->m_num; p < e; ++p)
        if(p->cmp(s, len))
            return p;
    return nullptr;
}

//-----------------------------------------------------------------------------
template< class T >
bool EnumSymbols< T >::Sym::cmp(const char *s) const
{
    if(strcmp(name, s) == 0)
        return true;

    auto o = eoffs< T >();
    if(o > 0)
        if(strncmp(name + o, s, o) == 0)
            return true;

    return false;
}

template< class T >
bool EnumSymbols< T >::Sym::cmp(const char *s, size_t len) const
{
    if(strncmp(name, s, len) == 0)
        return true;

    auto o = eoffs< T >();
    if(o > 0)
    {
        auto m = len < o ? len : o;
        if(strncmp(name + o, s, m) == 0)
            return true;
    }

    return false;
}

#endif // _C4_ENUM_HPP_
