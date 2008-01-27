#ifndef _OSDEP_ENDIAN_H
#define _OSDEP_ENDIAN_H

/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Michael Niedermayer <michaelni@gmx.at>
 * Copyright (C) 2007 Patrick Georgi <patrick@georgi-clan.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


/*
 * This file implements the Linux API (as of early 2007 at least)
 * for compile time endian checking, and provides support for
 * byte swap operations.
 *
 * Supported are: Linux, Solaris
 * Adding support for other operating systems is a matter of
 * figuring out how the system exposes information about the ISA
 * to the C preprocessor and use this to represent that information
 * The Linux Way.
 *
 * See Solaris as an example.
 */

 /* define __BYTE_ORDER to __LITTLE_ENDIAN or __BIG_ENDIAN as appropriate */
#ifdef linux
#include <endian.h>
#endif

#ifdef sun
#include <sys/isa_defs.h>

#ifdef _LITLE_ENDIAN
#define __BYTE_ORDER __LITTLE_ENDIAN
#else
#define __BYTE_ORDER __BIG_ENDIAN
#endif

#endif

 /* implement bswap_{16,32,64}.
  * A portable default implementation exists, but there might be
  * faster ways on your system.
  */
#ifdef linux
#include <byteswap.h>
#else
/* bswap_16, bswap_32, bswap_64: 
   taken from libavutil, under the following terms:

   copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at>
   LGPL v2.1 or later
   considered relicensed as GPL according to LGPL article 3
*/
static inline uint16_t bswap_16(uint16_t x){
    return (x>>8) | (x<<8);
}

static inline uint32_t bswap_32(uint32_t x){
    x= ((x<<8)&0xFF00FF00) | ((x>>8)&0x00FF00FF);
    return (x>>16) | (x<<16);
}

static inline uint64_t bswap_64(uint64_t x)
{
    union {
        uint64_t ll;
        uint32_t l[2];
    } w, r;
    w.ll = x;
    r.l[0] = bswap_32 (w.l[1]);
    r.l[1] = bswap_32 (w.l[0]);
    return r.ll;
}
#endif

#endif
