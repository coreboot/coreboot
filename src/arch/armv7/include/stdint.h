#ifndef ARM_STDINT_H
#define ARM_STDINT_H

#if defined(__GNUC__)
#define __HAVE_LONG_LONG__ 1
#else
#define __HAVE_LONG_LONG__ 0
#endif

/* Exact integral types */
typedef unsigned char      uint8_t;
typedef signed char        int8_t;

typedef unsigned short     uint16_t;
typedef signed short       int16_t;

typedef unsigned int       uint32_t;
typedef signed int         int32_t;

#if __HAVE_LONG_LONG__
typedef unsigned long long uint64_t;
typedef signed long long   int64_t;
#endif

/* Small types */
typedef unsigned char      uint_least8_t;
typedef signed char        int_least8_t;

typedef unsigned short     uint_least16_t;
typedef signed short       int_least16_t;

typedef unsigned int       uint_least32_t;
typedef signed int         int_least32_t;

#if __HAVE_LONG_LONG__
typedef unsigned long long uint_least64_t;
typedef signed long long   int_least64_t;
#endif

/* Fast Types */
typedef unsigned char      uint_fast8_t;
typedef signed char        int_fast8_t;

typedef unsigned int       uint_fast16_t;
typedef signed int         int_fast16_t;

typedef unsigned int       uint_fast32_t;
typedef signed int         int_fast32_t;

#if __HAVE_LONG_LONG__
typedef unsigned long long uint_fast64_t;
typedef signed long long   int_fast64_t;
#endif

/* Types for `void *' pointers.  */
typedef int                intptr_t;
typedef unsigned int       uintptr_t;

/* Largest integral types */
#if __HAVE_LONG_LONG__
typedef long long int      intmax_t;
typedef unsigned long long uintmax_t;
#else
typedef long int           intmax_t;
typedef unsigned long int  uintmax_t;
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#if __HAVE_LONG_LONG__
typedef uint64_t u64;
#endif
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;


#undef __HAVE_LONG_LONG__

#endif /* ARM_STDINT_H */
