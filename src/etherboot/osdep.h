#ifndef	__OSDEP_H__
#define __OSDEP_H__

/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 */

#define	__LITTLE_ENDIAN		/* x86 */

/* Taken from /usr/include/linux/hfs_sysdep.h */
#if defined(__BIG_ENDIAN)
#	if !defined(__constant_htonl)
#		define __constant_htonl(x) (x)
#	endif
#	if !defined(__constant_htons)
#		define __constant_htons(x) (x)
#	endif
#elif defined(__LITTLE_ENDIAN)
#	if !defined(__constant_htonl)
#		define __constant_htonl(x) \
        ((unsigned long int)((((unsigned long int)(x) & 0x000000ffU) << 24) | \
                             (((unsigned long int)(x) & 0x0000ff00U) <<  8) | \
                             (((unsigned long int)(x) & 0x00ff0000U) >>  8) | \
                             (((unsigned long int)(x) & 0xff000000U) >> 24)))
#	endif
#	if !defined(__constant_htons)
#		define __constant_htons(x) \
        ((unsigned short int)((((unsigned short int)(x) & 0x00ff) << 8) | \
                              (((unsigned short int)(x) & 0xff00) >> 8)))
#	endif
#else
#	error "Don't know if bytes are big- or little-endian!"
#endif

#define ntohl(x) \
(__builtin_constant_p(x) ? \
 __constant_htonl((x)) : \
 __swap32(x))
#define htonl(x) \
(__builtin_constant_p(x) ? \
 __constant_htonl((x)) : \
 __swap32(x))
#define ntohs(x) \
(__builtin_constant_p(x) ? \
 __constant_htons((x)) : \
 __swap16(x))
#define htons(x) \
(__builtin_constant_p(x) ? \
 __constant_htons((x)) : \
 __swap16(x))

static inline unsigned long int __swap32(unsigned long int x)
{
	__asm__("xchgb %b0,%h0\n\t"
		"rorl $16,%0\n\t"
		"xchgb %b0,%h0"
		: "=q" (x)
		: "0" (x));
	return x;
}

static inline unsigned short int __swap16(unsigned short int x)
{
	__asm__("xchgb %b0,%h0"
		: "=q" (x)
		: "0" (x));
	return x;
}

/* Make routines available to all */
#define	swap32(x)	__swap32(x)
#define	swap16(x)	__swap16(x)

/* Taken from asm/string-486.h */
#define __HAVE_ARCH_STRNCMP
extern inline int strncmp(const char * cs,const char * ct,int count)
{
register int __res;
__asm__ __volatile__(
	"\n1:\tdecl %3\n\t"
	"js 2f\n\t"
	"movb (%1),%b0\n\t"
	"incl %1\n\t"
	"cmpb %b0,(%2)\n\t"
	"jne 3f\n\t"
	"incl %2\n\t"
	"testb %b0,%b0\n\t"
	"jne 1b\n"
	"2:\txorl %0,%0\n\t"
	"jmp 4f\n"
	"3:\tmovl $1,%0\n\t"
	"jb 4f\n\t"
	"negl %0\n"
	"4:"
	:"=q" (__res), "=r" (cs), "=r" (ct), "=r" (count)
	:"1"  (cs), "2"  (ct),  "3" (count));
return __res;
}

#include "linux-asm-string.h"
#include "linux-asm-io.h"

typedef	unsigned long Address;

/* ANSI prototyping macro */
#ifdef	__STDC__
#define	P(x)	x
#else
#define	P(x)	()
#endif

#endif

/*
 * Local variables:
 *  c-basic-offset: 8
 * End:
 */
