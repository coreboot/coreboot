#ifndef _SWAB_H
#define _SWAB_H

/*
 * linux/byteorder/swab.h
 * Byte-swapping, independently from CPU endianness
 *	swabXX[ps]?(foo)
 *
 * Francois-Rene Rideau <fare@tunes.org> 19971205
 *    separated swab functions from cpu_to_XX,
 *    to clean up support for bizarre-endian architectures.
 *
 * See asm-i386/byteorder.h and suches for examples of how to provide
 * architecture-dependent optimized versions
 *
 */

#if !defined(__APPLE__) && !defined(__NetBSD__)
#define ntohl(x)	(is_big_endian() ? (uint32_t)(x) : swab32(x))
#define htonl(x)	(is_big_endian() ? (uint32_t)(x) : swab32(x))
#else
#include <arpa/inet.h>
#endif
#define ntohll(x)	(is_big_endian() ? (uint64_t)(x) : swab64(x))
#define htonll(x)	(is_big_endian() ? (uint64_t)(x) : swab64(x))

/* casts are necessary for constants, because we never know how for sure
 * how U/UL/ULL map to __u16, __u32, __u64. At least not in a portable way.
 */
#define swab16(x) \
	((unsigned short)( \
		(((unsigned short)(x) & (unsigned short)0x00ffU) << 8) | \
		(((unsigned short)(x) & (unsigned short)0xff00U) >> 8) ))

#define swab32(x) \
	((unsigned int)( \
		(((unsigned int)(x) & (unsigned int)0x000000ffUL) << 24) | \
		(((unsigned int)(x) & (unsigned int)0x0000ff00UL) <<  8) | \
		(((unsigned int)(x) & (unsigned int)0x00ff0000UL) >>  8) | \
		(((unsigned int)(x) & (unsigned int)0xff000000UL) >> 24) ))

#define swab64(x) \
	((uint64_t)( \
		(((uint64_t)(x) & (uint64_t)0x00000000000000ffULL) << 56) | \
		(((uint64_t)(x) & (uint64_t)0x000000000000ff00ULL) << 40) | \
		(((uint64_t)(x) & (uint64_t)0x0000000000ff0000ULL) << 24) | \
		(((uint64_t)(x) & (uint64_t)0x00000000ff000000ULL) <<  8) | \
		(((uint64_t)(x) & (uint64_t)0x000000ff00000000ULL) >>  8) | \
		(((uint64_t)(x) & (uint64_t)0x0000ff0000000000ULL) >> 24) | \
		(((uint64_t)(x) & (uint64_t)0x00ff000000000000ULL) >> 40) | \
		(((uint64_t)(x) & (uint64_t)0xff00000000000000ULL) >> 56) ))

/* common.c */
int is_big_endian(void);

#endif /* _SWAB_H */
