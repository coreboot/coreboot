/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011,2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC
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
 * Foundation, Inc.
 */

#ifndef CALLOUTS_AMD_AGESA_H
#define CALLOUTS_AMD_AGESA_H

#include "Porting.h"
#include "AGESA.h"

#define BIOS_HEAP_START_ADDRESS 	0x010000000
#define BIOS_HEAP_SIZE			0x30000
#define BSP_STACK_BASE_ADDR		0x30000

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


AGESA_STATUS agesa_AllocateBuffer (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS agesa_DeallocateBuffer (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS agesa_LocateBuffer (UINT32 Func, UINT32 Data, VOID *ConfigPtr);

AGESA_STATUS agesa_NoopUnsupported (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS agesa_NoopSuccess (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS agesa_EmptyIdsInitData (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS agesa_Reset (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS agesa_RunFuncOnAp (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS agesa_GfxGetVbiosImage(UINT32 Func, UINT32 FchData, VOID *ConfigPrt);

AGESA_STATUS agesa_ReadSpd (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS agesa_ReadSpd_from_cbfs(UINT32 Func, UINT32 Data, VOID *ConfigPtr);

AGESA_STATUS GetBiosCallout (UINT32 Func, UINT32 Data, VOID *ConfigPtr);

typedef struct {
	UINT32 CalloutName;
	AGESA_STATUS (*CalloutPtr) (UINT32 Func, UINT32 Data, VOID* ConfigPtr);
} BIOS_CALLOUT_STRUCT;

extern const BIOS_CALLOUT_STRUCT BiosCallouts[];
extern const int BiosCalloutsLen;

#endif /* CALLOUTS_AMD_AGESA_H */
