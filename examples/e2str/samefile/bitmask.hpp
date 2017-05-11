#ifndef _C4_BITMASK_HPP_
#define _C4_BITMASK_HPP_

#include "enum.hpp"

template< class T, class I >
size_t bits2str(I bits, char *str, size_t sz)
{
    size_t pos = 0;
    for(auto const& p : enum_pairs< T >())
    {
        I b(p.value);
        if((b != 0) && ((bits & b) == b))
        {
            bits &= ~b;
            // append bit-or character
            if(pos > 0)
            {
                if(pos < sz) str[pos] = '|';
                ++pos;
            }
            // append bit string
            size_t len = strlen(p.name);
            if(pos+len < sz) strncpy(str+pos, p.name, len);
            pos += len;
        }
    }
    assert(bits == 0);
    if(pos == 0) // make sure at least 0 is written
    {
        if(pos < sz) str[pos] = '0';
        pos++;
    }
    if(pos < sz) str[pos] = '\0';
    pos++;
    return pos;
}
template< class T, class I >
size_t bits2str_size(I bits)
{
    size_t pos = 0;
    for(auto const& p : enum_pairs< T >())
    {
        I b(p.value);
        if((b != 0) && ((bits & b) == b))
        {
            if(pos > 0) ++pos; // append bit-or character
            pos += strlen(p.name); // append bit string
        }
    }
    assert(bits == 0);
    if(pos == 0) ++pos; // 0
    ++pos; // terminating '\0'
    return pos;
}
template< class T, class Stream, class I >
Stream& bits2stream(I bits, Stream &s)
{
    bool written = false;
    for(auto const& p : enum_pairs< T >())
    {
        I b(p.value);
        if((b != 0) && ((bits & b) == b))
        {
            if(written) s << '|'; // append bit-or character
            s << p.name; // append bit string
            written = true;
            bits &= ~b;
        }
    }
    if(!written)
    {
        s << 0;
    }
}

#ifdef SHUT_UP_FOR_NOW
template< class T, class I >
I str2bits(const char *str, size_t sz)
{
    I val = 0;
    auto pairs = enum_pairs< T >();
    bool started = false;
    bool alnum = false, num = false;
    const char *f = nullptr;
    size_t n=0;
    for(const char *c = str; c < str + sz; ++c, ++n)
    {
        if((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z'))
        {
            assert(!num);
            if(!started)
            {
                f = c;
                alnum = started = true;
            }
        }
        else if(*c >= '0' && *c <= '9')
        {
            if(!started)
            {
                f = c;
                num = started = true;
            }
        }
        else if(*c == ':')
        {
        }
        else if(*c == '|')
        {
            assert(num != alnum);
            if(num)
            {
                int64_t tmp;
                snscanf(f, n, "%ld", &tmp);
                val |= (I)tmp;
            }
            else if(alnum)
            {
                auto *p = pairs.find(f, n);
                assert("find a valid enum pair name" && p != nullptr);
                val |= p->value;
            }
            f = nullptr;
            n = 0;
            started = num = alnum = false;
        }
        else if(*c == '\0')
        {
            break;
        }
        else
        {
            assert(!"bad character in bitmask string");
        }
    }
    assert(num != alnum);
    if(num)
    {
        int64_t tmp;
        snscanf(f, n, "%ld", &tmp);
        val |= (I)tmp;
    }
    else if(alnum)
    {
        auto *p = pairs.get(f, n);
        val |= p->value;
    }
    return val;
}
#endif // SHUT_UP_FOR_NOW

#include <type_traits>
template< class T, class I/* = typename std::underlying_type< T >::type */>
I str2bits(const char *str)
{
    return 0;//str2bits< T, I >(str, strlen(str));
}

#endif // _C4_BITMASK_HPP_
