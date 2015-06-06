/*
* Copyright (C) 2015 Broadcom Corporation
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation version 2.
*
* This program is distributed "as is" WITHOUT ANY WARRANTY of any
* kind, whether express or implied; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#ifndef __SOC_BROADCOM_CYGNUS_CYGNUS_TYPES_H__
#define __SOC_BROADCOM_CYGNUS_CYGNUS_TYPES_H__

#include <stdint.h>
#include <types.h>

#ifndef TRUE
#define TRUE (1 == 1)
#endif

#ifndef FALSE
#define FALSE (1 == 0)
#endif

#ifndef NULL
#define NULL	0
#endif

/**********************************************************************
 *  Basic types
 **********************************************************************/

typedef uint8_t  uint8;
typedef int8_t   int8;
typedef uint16_t uint16;
typedef int16_t  int16;
typedef int32_t  int32;
typedef uint32_t uint32;

#endif
