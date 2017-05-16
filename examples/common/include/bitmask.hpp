#ifndef _C4_BITMASK_HPP_
#define _C4_BITMASK_HPP_

#include <cinttypes>
#include <cstring>

#include "enum.hpp"

//-----------------------------------------------------------------------------
/** write a bitmask to a stream, formatted as a string */
template< class T, class Stream, class I >
Stream& bm2stream(I bits, Stream &s)
{
    bool written = false;
    for(auto const& p : esyms< T >())
    {
        I b(p.value);
        if((b != 0) && ((bits & b) == b))
        {
            if(written) s << '|'; // append bit-or character
            written = true;
            s << p.name; // append bit string
            bits &= ~b;
        }
    }
    if(!written)
    {
        s << 0;
    }
}

//-----------------------------------------------------------------------------
/** convert a bitmask to string.
 * return the number of characters written. To find the needed size,
 * call first with str=nullptr or with sz=0 */
template< class T, class I >
size_t bm2str(I bits, char *str = nullptr, size_t sz = 0)
{
// this macro simplifies the code
#define _c4appendchars(pushcode, num)           \
    if(str && (pos + num <= sz))                \
    {                                           \
        pushcode;                               \
    }                                           \
    else if(str && sz)                          \
    {                                           \
        C4_ERROR("cannot write to string pos=%d num=%d sz=%d", (int)pos, (int)num, (int)sz);     \
    }                                           \
    pos += num

    size_t pos = 0;
    typename EnumSymbols< T >::Sym const* zero = nullptr;
    for(auto const& p : esyms< T >())
    {
        I b(p.value);
        if(b == 0)
        {
            zero = &p; // save this symbol for later
        }
        else if((bits & b) == b)
        {
            bits &= ~b;
            // append bit-or character
            if(pos > 0)
            {
                _c4appendchars(str[pos] = '|', 1);
            }
            // append bit string
            size_t len = strlen(p.name);
            _c4appendchars(strncpy(str+pos, p.name, len), len);
        }
    }
    C4_ASSERT(bits == 0);
    if(pos == 0) // make sure at least something is written
    {
        if(zero) // if we have a zero symbol, use that
        {
            size_t len = strlen(zero->name);
            _c4appendchars(strncpy(str+pos, zero->name, len), len);
        }
        else // otherwise just write an integer zero
        {
            _c4appendchars(str[pos] = '0', 1);
        }
    }
    _c4appendchars(str[pos] = '\0', 1);

    return pos;

// cleanup!
#undef _c4appendchars
}

//-----------------------------------------------------------------------------

namespace detail {
template< class I > struct scanftag;
template<> struct scanftag< uint8_t  > { static constexpr const char *tag = "%" SCNu8 ; };
template<> struct scanftag< uint16_t > { static constexpr const char *tag = "%" SCNu16; };
template<> struct scanftag< uint32_t > { static constexpr const char *tag = "%" SCNu32; };
template<> struct scanftag< uint64_t > { static constexpr const char *tag = "%" SCNu64; };
template<> struct scanftag<  int8_t  > { static constexpr const char *tag = "%" SCNd8 ; };
template<> struct scanftag<  int16_t > { static constexpr const char *tag = "%" SCNd16; };
template<> struct scanftag<  int32_t > { static constexpr const char *tag = "%" SCNd32; };
template<> struct scanftag<  int64_t > { static constexpr const char *tag = "%" SCNd64; };
} // end namespace detail

/** convert a string to a bitmask */
template< class T, class I >
I str2bm(const char *str, size_t sz)
{
    I val = 0;
    auto pairs = esyms< T >();
    bool started = false;
    bool alnum = false, num = false;
    const char *f = nullptr;
    size_t n = 0;
    for(const char *c = str; c <= str + sz; ++c, ++n)
    {
        if((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') || *c == '_')
        {
            C4_CHECK(!num);
            if(!started)
            {
                f = c;
                n = 0;
                alnum = started = true;
            }
        }
        else if(*c >= '0' && *c <= '9')
        {
            C4_CHECK(!alnum);
            if(!started)
            {
                f = c;
                n = 0;
                num = started = true;
            }
        }
        else if(*c == ':' || *c == ' ')
        {
            // skip this char
        }
        else if(*c == '|' || *c == '\0')
        {
            C4_ASSERT(num != alnum);
            if(num)
            {
                I tmp;
                auto numconv = sscanf(f, detail::scanftag<I>::tag, &tmp);
                C4_CHECK_MSG(numconv == 1, "could not read string as an int: '%.*s'", (int)n, f);
                val |= tmp;
            }
            else if(alnum)
            {
                auto *p = pairs.find(f, n);
                C4_CHECK_MSG(p != nullptr, "no valid enum pair name for '%.*s'", (int)n, f);
                val |= p->value;
            }
            started = num = alnum = false;
            if(*c == '\0')
            {
                break;
            }
        }
        else
        {
            C4_ERROR("bad character '%c' in bitmask string", *c);
        }
    }
    return val;
}

/** convert a string to a bitmask */
template< class T, class I >
I str2bm(const char *str)
{
    return str2bm<T,I>(str, strlen(str));
}

#endif // _C4_BITMASK_HPP_
