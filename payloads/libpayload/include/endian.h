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

#define swap_bytes16(in) ((((in) & 0xFF) << 8) | (((in) & 0xFF00) >> 8))
#define swap_bytes32(in) ((((in) & 0xFF) << 24) | (((in) & 0xFF00) << 8) | \
			  (((in) & 0xFF0000) >> 8) | \
			  (((in) & 0xFF000000) >> 24))
#define swap_bytes64(in) (((uint64_t)swap_bytes32((uint32_t)(in)) << 32) | \
			  ((uint64_t)swap_bytes32((uint32_t)((in) >> 32))))


#if defined CONFIG_BIG_ENDIAN

#define htobew(in) (in)
#define htobel(in) (in)
#define htobell(in) (in)

#define htolew(in) swap_bytes16(in)
#define htolel(in) swap_bytes32(in)
#define htolell(in) swap_bytes64(in)

#elif defined CONFIG_LITTLE_ENDIAN

#define htobew(in) swap_bytes16(in)
#define htobel(in) swap_bytes32(in)
#define htobell(in) swap_bytes64(in)

#define htolew(in) (in)
#define htolel(in) (in)
#define htolell(in) (in)

#else

#error Cant tell if the CPU is little or big endian.

#endif

#define betohw(in) htobew(in)
#define betohl(in) htobel(in)
#define betohll(in) htobell(in)

#define letohw(in) htolew(in)
#define letohl(in) htolel(in)
#define letohll(in) htolell(in)

#define htonw(in) htobew(in)
#define htonl(in) htobel(in)
#define htonll(in) htobell(in)

#define ntohw(in) htonw(in)
#define ntohl(in) htonl(in)
#define ntohll(in) htonll(in)

#endif
