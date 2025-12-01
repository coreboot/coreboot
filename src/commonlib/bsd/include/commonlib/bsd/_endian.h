/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-only */

#ifndef _COMMONLIB_BSD__ENDIAN_H_
#define _COMMONLIB_BSD__ENDIAN_H_

/*
 * This header should not be included directly. Including source must define
 * prerequisites like uintXX_t types, the byteswap functions swabXX(),
 * __BIG_ENDIAN or __LITTLE_ENDIAN and I/O accessors readXX()/writeXX().
 */

/* Endian functions from glibc 2.9 / BSD "endian.h" */

#if defined(__BIG_ENDIAN)
	#define htobe16(in) (in)
	#define htobe32(in) (in)
	#define htobe64(in) (in)
	#define htole16(in) ((uint16_t)swab16(in))
	#define htole32(in) ((uint32_t)swab32(in))
	#define htole64(in) ((uint64_t)swab64(in))
#elif defined(__LITTLE_ENDIAN)
	#define htobe16(in) ((uint16_t)swab16(in))
	#define htobe32(in) ((uint32_t)swab32(in))
	#define htobe64(in) ((uint64_t)swab64(in))
	#define htole16(in) (in)
	#define htole32(in) (in)
	#define htole64(in) (in)
#else
	#error "Including source must #define __LITTLE_ENDIAN or __BIG_ENDIAN"
#endif

#define be16toh(in) htobe16(in)
#define be32toh(in) htobe32(in)
#define be64toh(in) htobe64(in)

#define le16toh(in) htole16(in)
#define le32toh(in) htole32(in)
#define le64toh(in) htole64(in)

#define htonw(in) htobe16(in)
#define htonl(in) htobe32(in)
#define htonll(in) htobe64(in)

#define ntohw(in) be16toh(in)
#define ntohl(in) be32toh(in)
#define ntohll(in) be64toh(in)

/*
 * RISC-V doesn't support misaligned access so it must use memcpy().
 */
#if defined(__COREBOOT__) && ENV_RISCV
#define __ENDIAN_RISCV_WORKAROUND 1
#else
#define __ENDIAN_RISCV_WORKAROUND 0
#endif

/*
 * be16dec/be32dec/be64dec/le16dec/le32dec/le64dec family of functions.
 */
#define DEFINE_ENDIAN_DEC(endian, width) \
	static inline uint##width##_t endian##width##dec(const void *p) \
	{ \
		if (__ENDIAN_RISCV_WORKAROUND) { \
			uint##width##_t val; \
			memcpy(&val, p, sizeof(val)); \
			return endian##width##toh(val); \
		} else { \
			return endian##width##toh(*(uint##width##_t *)p); \
		} \
	}

DEFINE_ENDIAN_DEC(be, 16)
DEFINE_ENDIAN_DEC(be, 32)
DEFINE_ENDIAN_DEC(be, 64)
DEFINE_ENDIAN_DEC(le, 16)
DEFINE_ENDIAN_DEC(le, 32)
DEFINE_ENDIAN_DEC(le, 64)

/*
 * be16enc/be32enc/be64enc/le16enc/le32enc/le64enc family of functions.
 */
#define DEFINE_ENDIAN_ENC(endian, width) \
	static inline void endian##width##enc(void *p, uint##width##_t u) \
	{ \
		if (__ENDIAN_RISCV_WORKAROUND) { \
			uint##width##_t val = hto##endian##width(u); \
			memcpy(p, &val, sizeof(val)); \
		} else { \
			*(uint##width##_t *)p = hto##endian##width(u); \
		} \
	}

DEFINE_ENDIAN_ENC(be, 16)
DEFINE_ENDIAN_ENC(be, 32)
DEFINE_ENDIAN_ENC(be, 64)
DEFINE_ENDIAN_ENC(le, 16)
DEFINE_ENDIAN_ENC(le, 32)
DEFINE_ENDIAN_ENC(le, 64)

/* read/write with uintptr_t address */
#define read8p(addr)		read8((void *)((uintptr_t)(addr)))
#define read16p(addr)		read16((void *)((uintptr_t)(addr)))
#define read32p(addr)		read32((void *)((uintptr_t)(addr)))
#define read64p(addr)		read64((void *)((uintptr_t)(addr)))
#define write8p(addr, value)	write8((void *)((uintptr_t)(addr)), value)
#define write16p(addr, value)	write16((void *)((uintptr_t)(addr)), value)
#define write32p(addr, value)	write32((void *)((uintptr_t)(addr)), value)
#define write64p(addr, value)	write64((void *)((uintptr_t)(addr)), value)

/* Handy bit manipulation macros */

#define __clrsetbits(endian, bits, addr, clear, set) \
	write##bits(addr, hto##endian##bits((endian##bits##toh( \
		read##bits(addr)) & ~((uint##bits##_t)(clear))) | (set)))

#define clrbits_le64(addr, clear)	__clrsetbits(le, 64, addr, clear, 0)
#define clrbits_be64(addr, clear)	__clrsetbits(be, 64, addr, clear, 0)
#define clrbits_le32(addr, clear)	__clrsetbits(le, 32, addr, clear, 0)
#define clrbits_be32(addr, clear)	__clrsetbits(be, 32, addr, clear, 0)
#define clrbits_le16(addr, clear)	__clrsetbits(le, 16, addr, clear, 0)
#define clrbits_be16(addr, clear)	__clrsetbits(be, 16, addr, clear, 0)

#define setbits_le64(addr, set)		__clrsetbits(le, 64, addr, 0, set)
#define setbits_be64(addr, set)		__clrsetbits(be, 64, addr, 0, set)
#define setbits_le32(addr, set)		__clrsetbits(le, 32, addr, 0, set)
#define setbits_be32(addr, set)		__clrsetbits(be, 32, addr, 0, set)
#define setbits_le16(addr, set)		__clrsetbits(le, 16, addr, 0, set)
#define setbits_be16(addr, set)		__clrsetbits(be, 16, addr, 0, set)

#define clrsetbits_le64(addr, clear, set) __clrsetbits(le, 64, addr, clear, set)
#define clrsetbits_be64(addr, clear, set) __clrsetbits(be, 64, addr, clear, set)
#define clrsetbits_le32(addr, clear, set) __clrsetbits(le, 32, addr, clear, set)
#define clrsetbits_be32(addr, clear, set) __clrsetbits(be, 32, addr, clear, set)
#define clrsetbits_le16(addr, clear, set) __clrsetbits(le, 16, addr, clear, set)
#define clrsetbits_be16(addr, clear, set) __clrsetbits(be, 16, addr, clear, set)

#define __clrsetbits_impl(bits, addr, clear, set) write##bits(addr, \
	(read##bits(addr) & ~((uint##bits##_t)(clear))) | (set))

#define clrsetbits8(addr, clear, set)	__clrsetbits_impl(8, addr, clear, set)
#define clrsetbits16(addr, clear, set)	__clrsetbits_impl(16, addr, clear, set)
#define clrsetbits32(addr, clear, set)	__clrsetbits_impl(32, addr, clear, set)
#define clrsetbits64(addr, clear, set)	__clrsetbits_impl(64, addr, clear, set)

#define setbits8(addr, set)		clrsetbits8(addr, 0, set)
#define setbits16(addr, set)		clrsetbits16(addr, 0, set)
#define setbits32(addr, set)		clrsetbits32(addr, 0, set)
#define setbits64(addr, set)		clrsetbits64(addr, 0, set)

#define clrbits8(addr, clear)		clrsetbits8(addr, clear, 0)
#define clrbits16(addr, clear)		clrsetbits16(addr, clear, 0)
#define clrbits32(addr, clear)		clrsetbits32(addr, clear, 0)
#define clrbits64(addr, clear)		clrsetbits64(addr, clear, 0)

#define clrsetbits8p(addr, clear, set)	clrsetbits8((void *)((uintptr_t)(addr)), clear, set)
#define clrsetbits16p(addr, clear, set)	clrsetbits16((void *)((uintptr_t)(addr)), clear, set)
#define clrsetbits32p(addr, clear, set)	clrsetbits32((void *)((uintptr_t)(addr)), clear, set)
#define clrsetbits64p(addr, clear, set)	clrsetbits64((void *)((uintptr_t)(addr)), clear, set)

#define setbits8p(addr, set)		clrsetbits8((void *)((uintptr_t)(addr)), 0, set)
#define setbits16p(addr, set)		clrsetbits16((void *)((uintptr_t)(addr)), 0, set)
#define setbits32p(addr, set)		clrsetbits32((void *)((uintptr_t)(addr)), 0, set)
#define setbits64p(addr, set)		clrsetbits64((void *)((uintptr_t)(addr)), 0, set)

#define clrbits8p(addr, clear)		clrsetbits8((void *)((uintptr_t)(addr)), clear, 0)
#define clrbits16p(addr, clear)		clrsetbits16((void *)((uintptr_t)(addr)), clear, 0)
#define clrbits32p(addr, clear)		clrsetbits32((void *)((uintptr_t)(addr)), clear, 0)
#define clrbits64p(addr, clear)		clrsetbits64((void *)((uintptr_t)(addr)), clear, 0)

#endif /* _COMMONLIB_BSD__ENDIAN_H_ */
