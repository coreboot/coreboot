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

#ifndef CALLOUTS_AMD_AGESA_FAM12_H
#define CALLOUTS_AMD_AGESA_FAM12_H

#include "Porting.h"
#include "AGESA.h"

#define BIOS_HEAP_START_ADDRESS			0x10000 /* HEAP during cold boot */
#define BIOS_HEAP_SIZE				0x20000
#define BSP_STACK_BASE_ADDR			0x30000

typedef struct _BIOS_HEAP_MANAGER {
	UINT32 StartOfAllocatedNodes;
	UINT32 StartOfFreedNodes;
} BIOS_HEAP_MANAGER;

typedef struct _BIOS_BUFFER_NODE {
	UINT32 BufferHandle;
	UINT32 BufferSize;
	UINT32 NextNodeOffset;
} BIOS_BUFFER_NODE;

UINT32 GetHeapBase(AMD_CONFIG_PARAMS *StdHeader);
void EmptyHeap(void);

/* REQUIRED CALLOUTS
 * AGESA ADVANCED CALLOUTS - CPU
 */
AGESA_STATUS BiosAllocateBuffer (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS BiosDeallocateBuffer (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS BiosLocateBuffer (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS BiosRunFuncOnAp (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS BiosReset (UINT32 Func, UINT32 Data, VOID *ConfigPtr);

/* AGESA ADVANCED CALLOUTS - MEMORY */
AGESA_STATUS BiosReadSpd (UINT32 Func,UINT32	Data,VOID *ConfigPtr);

/*	Call the host environment interface to provide a user hook opportunity. */
AGESA_STATUS BiosHookBeforeDramInit (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
/* PCIE slot reset control */
AGESA_STATUS BiosGnbPcieSlotReset (UINT32 Func, UINT32 Data, VOID *ConfigPtr);

#endif /* CALLOUTS_AMD_AGESA_FAM12_H */
