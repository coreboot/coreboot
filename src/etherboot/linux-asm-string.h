/*
 * Taken from Linux /usr/include/asm/string.h
 * All except memcpy, memmove, memset and memcmp removed.
 */

#ifndef	_I386_STRING_H_
#define _I386_STRING_H_

#include <types.h>
/*
 * This string-include defines all string functions as inline
 * functions. Use gcc. It also assumes ds=es=data space, this should be
 * normal. Most of the string-functions are rather heavily hand-optimized,
 * see especially strtok,strstr,str[c]spn. They should work, but are not
 * very easy to understand. Everything is done entirely within the register
 * set, making the functions fast and clean. String instructions have been
 * used through-out, making for "slightly" unclear code :-)
 *
 *		NO Copyright (C) 1991, 1992 Linus Torvalds,
 *		consider these trivial functions to be PD.
 */

extern void *__memcpy(void * to, const void * from, size_t n);
extern void *__constant_memcpy(void * to, const void * from, size_t n);
extern void *memmove(void * dest,const void * src, size_t n);
extern void *__memset_generic(void * s, char c,size_t count);
extern void *__constant_c_memset(void * s, unsigned long c, size_t count);
extern void *__constant_c_and_count_memset(void * s, unsigned long pattern, size_t count);


extern inline void * __memcpy(void * to, const void * from, size_t n)
{
int d0, d1, d2;
__asm__ __volatile__(
	"cld\n\t"
	"rep ; movsl\n\t"
	"testb $2,%b4\n\t"
	"je 1f\n\t"
	"movsw\n"
	"1:\ttestb $1,%b4\n\t"
	"je 2f\n\t"
	"movsb\n"
	"2:"
	: "=&c" (d0), "=&D" (d1), "=&S" (d2)
	:"0" (n/4), "q" (n),"1" ((long) to),"2" ((long) from)
	: "memory");
return (to);
}

/*
 * This looks horribly ugly, but the compiler can optimize it totally,
 * as the count is constant.
 */
extern inline void * __constant_memcpy(void * to, const void * from, size_t n)
{
	switch (n) {
		case 0:
			return to;
		case 1:
			*(unsigned char *)to = *(const unsigned char *)from;
			return to;
		case 2:
			*(unsigned short *)to = *(const unsigned short *)from;
			return to;
		case 3:
			*(unsigned short *)to = *(const unsigned short *)from;
			*(2+(unsigned char *)to) = *(2+(const unsigned char *)from);
			return to;
		case 4:
			*(unsigned long *)to = *(const unsigned long *)from;
			return to;
		case 6:	/* for Ethernet addresses */
			*(unsigned long *)to = *(const unsigned long *)from;
			*(2+(unsigned short *)to) = *(2+(const unsigned short *)from);
			return to;
		case 8:
			*(unsigned long *)to = *(const unsigned long *)from;
			*(1+(unsigned long *)to) = *(1+(const unsigned long *)from);
			return to;
		case 12:
			*(unsigned long *)to = *(const unsigned long *)from;
			*(1+(unsigned long *)to) = *(1+(const unsigned long *)from);
			*(2+(unsigned long *)to) = *(2+(const unsigned long *)from);
			return to;
		case 16:
			*(unsigned long *)to = *(const unsigned long *)from;
			*(1+(unsigned long *)to) = *(1+(const unsigned long *)from);
			*(2+(unsigned long *)to) = *(2+(const unsigned long *)from);
			*(3+(unsigned long *)to) = *(3+(const unsigned long *)from);
			return to;
		case 20:
			*(unsigned long *)to = *(const unsigned long *)from;
			*(1+(unsigned long *)to) = *(1+(const unsigned long *)from);
			*(2+(unsigned long *)to) = *(2+(const unsigned long *)from);
			*(3+(unsigned long *)to) = *(3+(const unsigned long *)from);
			*(4+(unsigned long *)to) = *(4+(const unsigned long *)from);
			return to;
	}
#define COMMON(x) \
__asm__ __volatile__( \
	"cld\n\t" \
	"rep ; movsl" \
	x \
	: "=&c" (d0), "=&D" (d1), "=&S" (d2) \
	: "0" (n/4),"1" ((long) to),"2" ((long) from) \
	: "memory");
{
	int d0, d1, d2;
	switch (n % 4) {
		case 0: COMMON(""); return to;
		case 1: COMMON("\n\tmovsb"); return to;
		case 2: COMMON("\n\tmovsw"); return to;
		default: COMMON("\n\tmovsw\n\tmovsb"); return to;
	}
}

#undef COMMON
}

#define __HAVE_ARCH_MEMCPY
#define memcpy(t, f, n) \
(__builtin_constant_p(n) ? \
 __constant_memcpy((t),(f),(n)) : \
 __memcpy((t),(f),(n)))

#define __HAVE_ARCH_MEMMOVE
extern inline void * memmove(void * dest,const void * src, size_t n)
{
int d0, d1, d2;
if (dest<src)
__asm__ __volatile__(
	"cld\n\t"
	"rep\n\t"
	"movsb"
	: "=&c" (d0), "=&S" (d1), "=&D" (d2)
	:"0" (n),"1" (src),"2" (dest)
	: "memory");
else
__asm__ __volatile__(
	"std\n\t"
	"rep\n\t"
	"movsb\n\t"
	"cld"
	: "=&c" (d0), "=&S" (d1), "=&D" (d2)
	:"0" (n),
	 "1" (n-1+(const char *)src),
	 "2" (n-1+(char *)dest)
	:"memory");
return dest;
}

#define memcmp __builtin_memcmp

extern inline void * __memset_generic(void * s, char c,size_t count)
{
int d0, d1;
__asm__ __volatile__(
	"cld\n\t"
	"rep\n\t"
	"stosb"
	: "=&c" (d0), "=&D" (d1)
	:"a" (c),"1" (s),"0" (count)
	:"memory");
return s;
}

/* we might want to write optimized versions of these later */
#define __constant_count_memset(s,c,count) __memset_generic((s),(c),(count))

/*
 * memset(x,0,y) is a reasonably common thing to do, so we want to fill
 * things 32 bits at a time even when we don't know the size of the
 * area at compile-time..
 */
extern inline void * __constant_c_memset(void * s, unsigned long c, size_t count)
{
int d0, d1;
__asm__ __volatile__(
	"cld\n\t"
	"rep ; stosl\n\t"
	"testb $2,%b3\n\t"
	"je 1f\n\t"
	"stosw\n"
	"1:\ttestb $1,%b3\n\t"
	"je 2f\n\t"
	"stosb\n"
	"2:"
	: "=&c" (d0), "=&D" (d1)
	:"a" (c), "q" (count), "0" (count/4), "1" ((long) s)
	:"memory");
return (s);
}

/*
 * This looks horribly ugly, but the compiler can optimize it totally,
 * as we by now know that both pattern and count is constant..
 */
extern inline void * __constant_c_and_count_memset(void * s, unsigned long pattern, size_t count)
{
	switch (count) {
		case 0:
			return s;
		case 1:
			*(unsigned char *)s = pattern;
			return s;
		case 2:
			*(unsigned short *)s = pattern;
			return s;
		case 3:
			*(unsigned short *)s = pattern;
			*(2+(unsigned char *)s) = pattern;
			return s;
		case 4:
			*(unsigned long *)s = pattern;
			return s;
	}
#define COMMON(x) \
__asm__  __volatile__("cld\n\t" \
	"rep ; stosl" \
	x \
	: "=&c" (d0), "=&D" (d1) \
	: "a" (pattern),"0" (count/4),"1" ((long) s) \
	: "memory")
{
	int d0, d1;
	switch (count % 4) {
		case 0: COMMON(""); return s;
		case 1: COMMON("\n\tstosb"); return s;
		case 2: COMMON("\n\tstosw"); return s;
		default: COMMON("\n\tstosw\n\tstosb"); return s;
	}
}

#undef COMMON
}

#define __constant_c_x_memset(s, c, count) \
(__builtin_constant_p(count) ? \
 __constant_c_and_count_memset((s),(c),(count)) : \
 __constant_c_memset((s),(c),(count)))

#define __memset(s, c, count) \
(__builtin_constant_p(count) ? \
 __constant_count_memset((s),(c),(count)) : \
 __memset_generic((s),(c),(count)))

#define __HAVE_ARCH_MEMSET
#define memset(s, c, count) \
(__builtin_constant_p(c) ? \
 __constant_c_x_memset((s),(c),(count)) : \
 __memset((s),(c),(count)))

#endif
