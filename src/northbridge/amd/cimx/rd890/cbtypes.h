/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 - 2012 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef _CBTYPES_H_
#define _CBTYPES_H_

//#include <stdint.h>

/*
typedef int64_t __int64;
typedef void VOID;
typedef uint32_t UINTN;//
typedef int8_t CHAR8;
typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint64_t UINT64;
*/
typedef signed long long __int64;
typedef void VOID;
typedef unsigned int UINTN;//
typedef signed char CHAR8;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef signed int INT32;
typedef unsigned long long UINT64;

#define TRUE  1
#define FALSE 0
typedef unsigned char BOOLEAN;

#ifndef VOLATILE
#define VOLATILE volatile
#endif

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

//porting.h
#ifndef CONST
#define CONST const
#endif
#ifndef STATIC
#define STATIC static
#endif
#ifndef VOLATILE
#define VOLATILE volatile
#endif

#endif
