/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * linux/byteorder/swab.h
 * Byte-swapping, independently from CPU endianness
 *	swabXX[ps]?(foo)
 *
 * Francois-Rene Rideau <fare@tunes.org> 19971205
 *    separated swab functions from cpu_to_XX,
 *    to clean up support for bizarre-endian architectures.
 *
 * See asm-i386/byteorder.h and such for examples of how to provide
 * architecture-dependent optimized versions
 *
 */

/* casts are necessary for constants, because we never know how for sure
 * how U/UL/ULL map to __u16, __u32, __u64. At least not in a portable way.
 */

#ifndef _SWAB_H
#define _SWAB_H

#include <stdint.h>

#if ENV_ARMV4
#define swab16(x) \
	((unsigned short)( \
		(((unsigned short)(x) & (unsigned short)0x00ffU) << 8) | \
		(((unsigned short)(x) & (unsigned short)0xff00U) >> 8)))

#define swab32(x) \
	((unsigned int)( \
		(((unsigned int)(x) & 0x000000ffUL) << 24) | \
		(((unsigned int)(x) & 0x0000ff00UL) <<  8) | \
		(((unsigned int)(x) & 0x00ff0000UL) >>  8) | \
		(((unsigned int)(x) & 0xff000000UL) >> 24)))

#define swab64(x) \
	((uint64_t)( \
		(((uint64_t)(x) & (uint64_t)0x00000000000000ffULL) << 56) | \
		(((uint64_t)(x) & (uint64_t)0x000000000000ff00ULL) << 40) | \
		(((uint64_t)(x) & (uint64_t)0x0000000000ff0000ULL) << 24) | \
		(((uint64_t)(x) & (uint64_t)0x00000000ff000000ULL) <<  8) | \
		(((uint64_t)(x) & (uint64_t)0x000000ff00000000ULL) >>  8) | \
		(((uint64_t)(x) & (uint64_t)0x0000ff0000000000ULL) >> 24) | \
		(((uint64_t)(x) & (uint64_t)0x00ff000000000000ULL) >> 40) | \
		(((uint64_t)(x) & (uint64_t)0xff00000000000000ULL) >> 56)))
#else	/* ENV_ARMV4 */
#define swab16(x) ((uint16_t)__builtin_bswap16(x))
#define swab32(x) ((uint32_t)__builtin_bswap32(x))
#define swab64(x) ((uint64_t)__builtin_bswap64(x))
#endif	/* !ENV_ARMV4 */

#endif /* _SWAB_H */
