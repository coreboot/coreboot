/*
 * This file is part of the libpayload project.
 *
 * Copyright (c) 2012 The Chromium OS Authors.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#include <arch/io.h>
#include <arch/types.h>
#include <libpayload-config.h>

static inline uint16_t swap_bytes16(uint16_t in)
{
	return ((in & 0xFF) << 8) | ((in & 0xFF00) >> 8);
}

static inline uint32_t swap_bytes32(uint32_t in)
{
	return ((in & 0xFF) << 24) | ((in & 0xFF00) << 8) |
		((in & 0xFF0000) >> 8) | ((in & 0xFF000000) >> 24);
}

static inline uint64_t swap_bytes64(uint64_t in)
{
	return ((uint64_t)swap_bytes32((uint32_t)in) << 32) |
		((uint64_t)swap_bytes32((uint32_t)(in >> 32)));
}

/* Endian functions from glibc 2.9 / BSD "endian.h" */

#if IS_ENABLED(CONFIG_LP_BIG_ENDIAN)

#define htobe16(in) (in)
#define htobe32(in) (in)
#define htobe64(in) (in)

#define htole16(in) swap_bytes16(in)
#define htole32(in) swap_bytes32(in)
#define htole64(in) swap_bytes64(in)

#elif IS_ENABLED(CONFIG_LP_LITTLE_ENDIAN)

#define htobe16(in) swap_bytes16(in)
#define htobe32(in) swap_bytes32(in)
#define htobe64(in) swap_bytes64(in)

#define htole16(in) (in)
#define htole32(in) (in)
#define htole64(in) (in)

#else

#error Cant tell if the CPU is little or big endian.

#endif /* CONFIG_*_ENDIAN */

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
 * Alignment-agnostic encode/decode bytestream to/from little/big endian.
 */

static inline uint16_t be16dec(const void *pp)
{
	uint8_t const *p = (uint8_t const *)pp;

	return ((p[0] << 8) | p[1]);
}

static inline uint32_t be32dec(const void *pp)
{
	uint8_t const *p = (uint8_t const *)pp;

	return (((unsigned)p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
}

static inline uint16_t le16dec(const void *pp)
{
	uint8_t const *p = (uint8_t const *)pp;

	return ((p[1] << 8) | p[0]);
}

static inline uint32_t le32dec(const void *pp)
{
	uint8_t const *p = (uint8_t const *)pp;

	return ((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0]);
}

static inline void bebitenc(void *pp, uint32_t u, uint8_t b)
{
	uint8_t *p = (uint8_t *)pp;
	int i;

	for (i = (b - 1); i >= 0; i++)
		p[i] = (u >> i*8) & 0xFF;
}

static inline void be16enc(void *pp, uint16_t u)
{
	bebitenc(pp, u, 2);
}

static inline void be32enc(void *pp, uint32_t u)
{
	bebitenc(pp, u, 4);
}

static inline void lebitenc(void *pp, uint32_t u, uint8_t b)
{
	uint8_t *p = (uint8_t *)pp;
	int i;

	for (i = 0; i < b; i++)
		p[i] = (u >> i*8) & 0xFF;
}

static inline void le16enc(void *pp, uint16_t u)
{
	lebitenc(pp, u, 2);
}

static inline void le32enc(void *pp, uint32_t u)
{
	lebitenc(pp, u, 4);
}

/* Deprecated names (not in glibc / BSD) */
#define htobew(in) htobe16(in)
#define htobel(in) htobe32(in)
#define htobell(in) htobe64(in)
#define htolew(in) htole16(in)
#define htolel(in) htole32(in)
#define htolell(in) htole64(in)
#define betohw(in) be16toh(in)
#define betohl(in) be32toh(in)
#define betohll(in) be64toh(in)
#define letohw(in) le16toh(in)
#define letohl(in) le32toh(in)
#define letohll(in) le64toh(in)

/* Handy bit manipulation macros */

#define clrsetbits_le32(addr, clear, set) writel(htole32((le32toh(readl(addr)) \
	& ~(clear)) | (set)), (addr))
#define setbits_le32(addr, set) writel(htole32(le32toh(readl(addr)) \
	| (set)), (addr))
#define clrbits_le32(addr, clear) writel(htole32(le32toh(readl(addr)) \
	& ~(clear)), (addr))

#define clrsetbits_be32(addr, clear, set) writel(htobe32((be32toh(readl(addr)) \
	& ~(clear)) | (set)), (addr))
#define setbits_be32(addr, set) writel(htobe32(be32toh(readl(addr)) \
	| (set)), (addr))
#define clrbits_be32(addr, clear) writel(htobe32(be32toh(readl(addr)) \
	& ~(clear)), (addr))

#endif /* _ENDIAN_H_ */
