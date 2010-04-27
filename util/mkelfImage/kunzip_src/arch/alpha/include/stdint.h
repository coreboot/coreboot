#ifndef ALPHA_STDINT_H
#define ALPHA_STDINT_H

/* Exact integral types */
typedef unsigned char      uint8_t;
typedef signed char        int8_t;

typedef unsigned short     uint16_t;
typedef signed short       int16_t;

typedef unsigned int       uint32_t;
typedef signed int         int32_t;

typedef unsigned long      uint64_t;
typedef signed long        int64_t;


/* Small types */
typedef unsigned char      uint_least8_t;
typedef signed char        int_least8_t;

typedef unsigned short     uint_least16_t;
typedef signed short       int_least16_t;

typedef unsigned int       uint_least32_t;
typedef signed int         int_least32_t;

typedef unsigned long      uint_least64_t;
typedef signed long        int_least64_t;

/* Fast Types */
typedef unsigned char      uint_fast8_t;
typedef signed char        int_fast8_t;

typedef unsigned long      uint_fast16_t;
typedef signed long        int_fast16_t;

typedef unsigned long      uint_fast32_t;
typedef signed long        int_fast32_t;

typedef unsigned long      uint_fast64_t;
typedef signed long        int_fast64_t;

/* Types for `void *' pointers.  */
typedef long               intptr_t;
typedef unsigned long      uintptr_t;

/* Largest integral types */
typedef long               intmax_t;
typedef unsigned long      uintmax_t;


#endif /* ALPHA_STDINT_H */
