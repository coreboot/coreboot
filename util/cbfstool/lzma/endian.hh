#ifndef bqtEndianHH
#define bqtEndianHH

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS /* for UINT16_C etc */
#endif

#include <stdint.h>

#if defined(__x86_64)||defined(__i386)
#define LITTLE_ENDIAN_AND_UNALIGNED_ACCESS_OK
#else
#undef LITTLE_ENDIAN_AND_UNALIGNED_ACCESS_OK
#endif

#ifdef WIN32
# define LL_FMT "I64"
#else
# define LL_FMT "ll"
#endif


static inline uint_fast16_t get_8(const void* p)
{
    const unsigned char* data = (const unsigned char*)p;
    return data[0];
}
static inline uint_fast16_t get_16(const void* p)
{
  #ifdef LITTLE_ENDIAN_AND_UNALIGNED_ACCESS_OK
    return *(const uint_least16_t*)p;
  #else
    const unsigned char* data = (const unsigned char*)p;
    return get_8(data)  | (get_8(data+1) << UINT16_C(8));
  #endif
}
static inline uint_fast16_t R16r(const void* p)
{
  #ifdef BIG_ENDIAN_AND_UNALIGNED_ACCESS_OK
    return *(const uint_least16_t*)p;
  #else
    const unsigned char* data = (const unsigned char*)p;
    return get_8(data+1)  | (get_8(data) << UINT16_C(8));
  #endif
}
static inline uint_fast32_t R24(const void* p)
{
    /* Note: This might be faster if implemented through R32 and a bitwise and,
     * but we cannot do that because we don't know if the third byte is a valid
     * memory location.
     */
    const unsigned char* data = (const unsigned char*)p;
    return get_16(data) | (get_8(data+2) << UINT32_C(16));
}
static inline uint_fast32_t R24r(const void* p)
{
    const unsigned char* data = (const unsigned char*)p;
    return get_16(data+1) | (get_8(data) << UINT32_C(16));
}
static inline uint_fast32_t get_32(const void* p)
{
  #ifdef LITTLE_ENDIAN_AND_UNALIGNED_ACCESS_OK
    return *(const uint_least32_t*)p;
  #else
    const unsigned char* data = (const unsigned char*)p;
    return get_16(data) | (get_16(data+2) << UINT32_C(16));
  #endif
}
static inline uint_fast32_t R32r(const void* p)
{
  #ifdef BIG_ENDIAN_AND_UNALIGNED_ACCESS_OK
    return *(const uint_least32_t*)p;
  #else
    const unsigned char* data = (const unsigned char*)p;
    return get_16(data+2) | (get_16(data) << UINT32_C(16));
  #endif
}

#define L (uint_fast64_t)

static inline uint_fast64_t get_64(const void* p)
{
  #ifdef LITTLE_ENDIAN_AND_UNALIGNED_ACCESS_OK
    return *(const uint_least64_t*)p;
  #else
    const unsigned char* data = (const unsigned char*)p;
    return (L get_32(data)) | ((L get_32(data+4)) << UINT64_C(32));
  #endif
}
static inline uint_fast64_t R64r(const void* p)
{
  #ifdef BIG_ENDIAN_AND_UNALIGNED_ACCESS_OK
    return *(const uint_least64_t*)p;
  #else
    const unsigned char* data = (const unsigned char*)p;
    return (L get_32(data+4)) | ((L get_32(data)) << UINT64_C(32));
  #endif
}

#undef L

static inline uint_fast64_t get_n(const void* p, unsigned bytes)
{
    const unsigned char* data = (const unsigned char*)p;
    uint_fast64_t res(0);
    switch(bytes)
    {
        case 8: return get_64(p);
        case 4: return get_32(p);
        case 2: return get_16(p);
        case 7: res |= ((uint_fast64_t)get_8(data+6)) << 48;
        case 6: res |= ((uint_fast64_t)get_8(data+5)) << 40;
        case 5: res |= ((uint_fast64_t)get_16(data+3)) << 24;
        case 3: res |= ((uint_fast64_t)get_16(data+1)) << 8;
        case 1: res |= get_8(data);
    }
    return res;
}

static void put_8(void* p, uint_fast8_t value)
{
    unsigned char* data = (unsigned char*)p;
    data[0] = value;
}
static void put_16(void* p, uint_fast16_t value)
{
  #ifdef LITTLE_ENDIAN_AND_UNALIGNED_ACCESS_OK
    *(uint_least16_t*)p = value;
  #else
    unsigned char* data = (unsigned char*)p;
    put_8(data+0, value   );
    put_8(data+1, value>>8);
  #endif
}
static void W24(void* p, uint_fast32_t value)
{
    unsigned char* data = (unsigned char*)p;
    put_16(data+0, value);
    put_8(data+2,  value >> UINT32_C(16));
}
static void put_32(void* p, uint_fast32_t value)
{
  #ifdef LITTLE_ENDIAN_AND_UNALIGNED_ACCESS_OK
    *(uint_least32_t*)p = value;
  #else
    unsigned char* data = (unsigned char*)p;
    put_16(data+0, value);
    put_16(data+2, value >> UINT32_C(16));
  #endif
}
static void put_64(void* p, uint_fast64_t value)
{
  #ifdef LITTLE_ENDIAN_AND_UNALIGNED_ACCESS_OK
    *(uint_least64_t*)p = value;
  #else
    unsigned char* data = (unsigned char*)p;
    put_32(data+0, (value));
    put_32(data+4, (value >> UINT64_C(32)));
  #endif
}

static inline void put_n(void* p, uint_fast64_t value, unsigned bytes)
{
    unsigned char* data = (unsigned char*)p;
    switch(bytes)
    {
        case 8: put_64(p, value); break;
        case 7: put_8(data+6, value>>48);
        case 6: put_8(data+5, value>>40);
        case 5: put_8(data+4, value>>32);
        case 4: put_32(p, value); break;
        case 3: W24(p, value); break;
        case 2: put_16(p, value); break;
        case 1: put_8(p, value); break;
    }
}

#endif
