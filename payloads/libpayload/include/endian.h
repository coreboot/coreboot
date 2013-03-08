/*
 * Copyright (c) 2012 The Chromium OS Authors.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _ENDIAN_H_
#define _ENDIAN_H_

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

#if defined CONFIG_BIG_ENDIAN

#define htobe16(in) (in)
#define htobe32(in) (in)
#define htobe64(in) (in)

#define htole16(in) swap_bytes16(in)
#define htole32(in) swap_bytes32(in)
#define htole64(in) swap_bytes64(in)

#elif defined CONFIG_LITTLE_ENDIAN

#define htobe16(in) swap_bytes16(in)
#define htobe32(in) swap_bytes32(in)
#define htobe64(in) swap_bytes64(in)

#define htole16(in) (in)
#define htole32(in) (in)
#define htole64(in) (in)

#else

#error Cant tell if the CPU is little or big endian.

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

#endif
