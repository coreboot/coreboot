/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _CBTYPES_H_
#define _CBTYPES_H_

typedef signed long long __int64;
typedef void VOID;
typedef unsigned int UINTN;
typedef signed char CHAR8;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef signed int INT32;
typedef unsigned long long UINT64;
typedef unsigned char BOOLEAN;

#define DMSG_SB_TRACE   0x02
#define TRACE(Arguments)

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef VOLATILE
#define VOLATILE volatile
#endif
#ifndef CONST
#define CONST const
#endif

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif


#ifndef STATIC
#define STATIC static
#endif
#ifndef VOLATILE
#define VOLATILE volatile
#endif

#endif
