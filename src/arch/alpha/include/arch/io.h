#ifndef ALPHA_IO_H
#define ALPHA_IO_H

#include <arch/compiler.h>

#define mb() \
__asm__ __volatile__("mb": : :"memory")

#define rmb() \
__asm__ __volatile__("mb": : :"memory")

#define wmb() \
__asm__ __volatile__("wmb": : :"memory")

/*
 * Virtual -> physical identity mapping starts at this offset
 */
#ifdef USE_48_BIT_KSEG
#define IDENT_ADDR     0xffff800000000000
#else
#define IDENT_ADDR     0xfffffc0000000000
#endif

#if defined(USE_CORE_TSUNAMI)
#include <northbridge/alpha/tsunami/core_tsunami.h>
#else
#error "What system is this?"
#endif
#endif /* ALPHA_IO_H */
