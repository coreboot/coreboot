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

#include <stdint.h>

inline static uint16_t _be16toh(const uint16_t value)
{
	const uint8_t *v = (const uint8_t *)&value;
	return v[0] << 8 | v[1];
}

static inline uint32_t _be32toh(const uint32_t value)
{
	const uint8_t *v = (const uint8_t *)&value;
	return v[0] << 24 | v[1] << 16 | v[2] << 8 | v[3];
}

static inline uint64_t _be64toh(const uint64_t value)
{
	const uint32_t *v = (const uint32_t *)&value;
	return (uint64_t)_be32toh(v[0]) << 32 | _be32toh(v[1]);
}

static inline uint16_t _le16toh(const uint16_t value)
{
	const uint8_t *v = (const uint8_t *)&value;
	return v[1] << 8 | v[0];
}

static inline uint32_t _le32toh(const uint32_t value)
{
	const uint8_t *v = (const uint8_t *)&value;
	return v[3] << 24 | v[2] << 16 | v[1] << 8 | v[0];
}

static inline uint64_t _le64toh(const uint64_t value)
{
	const uint32_t *v = (const uint32_t *)&value;
	return (uint64_t)_le32toh(v[1]) << 32 | _le32toh(v[0]);
}

/* Endian functions from glibc 2.9 / BSD "endian.h" */

#define htobe16(in) be16toh(in)
#define htobe32(in) be32toh(in)
#define htobe64(in) be64toh(in)

#define htole16(in) le16toh(in)
#define htole32(in) le32toh(in)
#define htole64(in) le64toh(in)

#define be16toh(in) _be16toh(in)
#define be32toh(in) _be32toh(in)
#define be64toh(in) _be64toh(in)

#define le16toh(in) _le16toh(in)
#define le32toh(in) _le32toh(in)
#define le64toh(in) _le64toh(in)

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
