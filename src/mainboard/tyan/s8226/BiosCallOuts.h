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

#ifndef _BIOS_CALLOUT_H_
#define _BIOS_CALLOUT_H_

#include "Porting.h"
#include "AGESA.h"

#define BIOS_HEAP_START_ADDRESS		0x00010000
#define BIOS_HEAP_SIZE			0x20000   /* 64MB */

typedef struct _BIOS_HEAP_MANAGER {
	//UINT32 AvailableSize;
	UINT32 StartOfAllocatedNodes;
	UINT32 StartOfFreedNodes;
} BIOS_HEAP_MANAGER;

typedef struct _BIOS_BUFFER_NODE {
	UINT32 BufferHandle;
	UINT32 BufferSize;
	UINT32 NextNodeOffset;
} BIOS_BUFFER_NODE;

/*
 * CALLOUTS
 */
AGESA_STATUS GetBiosCallout (UINT32 Func, UINT32 Data, VOID *ConfigPtr);

/* REQUIRED CALLOUTS
 * AGESA ADVANCED CALLOUTS - CPU
 */
AGESA_STATUS BiosAllocateBuffer (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS BiosDeallocateBuffer (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS BiosLocateBuffer (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS BiosRunFuncOnAp (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS BiosReset (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS BiosGetIdsInitData (UINT32 Func, UINT32 Data, VOID *ConfigPtr);

/* AGESA ADVANCED CALLOUTS - MEMORY */
AGESA_STATUS BiosReadSpd (UINT32  Func,UINT32  Data,VOID *ConfigPtr);

/* BIOS DEFAULT RET */
AGESA_STATUS BiosDefaultRet (UINT32 Func, UINT32 Data, VOID *ConfigPtr);

/*  Call the host environment interface to provide a user hook opportunity. */
AGESA_STATUS BiosHookBeforeDQSTraining (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
/*  Call the host environment interface to provide a user hook opportunity. */
AGESA_STATUS BiosHookBeforeDramInit (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
/*  Call the host environment interface to provide a user hook opportunity. */
AGESA_STATUS BiosHookBeforeExitSelfRefresh (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
#define SB_GPIO_REG02   2
#define SB_GPIO_REG09   9
#define SB_GPIO_REG10   10
#define SB_GPIO_REG15   15
#define SB_GPIO_REG17   17
#define SB_GPIO_REG21   21
#define SB_GPIO_REG25   25
#define SB_GPIO_REG28   28
#endif //_BIOS_CALLOUT_H_

