/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#ifndef _AMD_SB_CIMx_PLATFORM_H_
#define _AMD_SB_CIMx_PLATFORM_H_

#pragma pack(push,1)

#include <cpu/amd/common/cbtypes.h>
#include <console/console.h>
#include <console/loglevel.h>
#ifdef NULL
#undef NULL
#endif
#define NULL            0

typedef struct _EXT_PCI_ADDR{
	UINT32                  Reg :16;
	UINT32                  Func:3;
	UINT32                  Dev :5;
	UINT32                  Bus :8;
}EXT_PCI_ADDR;


typedef union _PCI_ADDR{
	UINT32                  ADDR;
	EXT_PCI_ADDR            Addr;
}PCI_ADDR;


#ifdef CIM_DEBUG

#if   CIM_DEBUG & 2
void    TraceDebug( UINT32 Level, CHAR8 *Format, ...);
#define TRACE(Arguments) TraceDebug Arguments
#else
#define TRACE(Arguments)
#endif

#if   CIM_DEBUG & 1
void    TraceCode ( UINT32 Level, UINT32 Code);
#define TRACECODE(Arguments) TraceCode Arguments
#else
#define TRACECODE(Arguments)
#endif
#else
	#ifdef TRACE
		#undef TRACE
	#endif
	#if CONFIG_REDIRECT_SBCIMX_TRACE_TO_SERIAL
		#define TRACE(Arguments) printk Arguments
	#else
		#define TRACE(Arguments) do {} while(0)
	#endif
	#define TRACECODE(Arguments)
#endif

#define FIXUP_PTR(ptr)  ptr

#pragma pack(pop)

#include "OEM.h"
#include "Amd.h"
#include "ACPILIB.h"
#include "SBTYPE.h"
#include "sbAMDLIB.h"
#include "SBCMNLIB.h"
#include "SB700.h"
#include "SBDEF.h"

#define DMSG_SB_TRACE   0x02

#endif  //#ifndef _AMD_SB_CIMx_PLATFORM_H_

