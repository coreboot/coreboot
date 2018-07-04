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
 */

#ifndef __CALLOUTS_AMD_AGESA_H__
#define __CALLOUTS_AMD_AGESA_H__

#include <amdblocks/agesawrapper.h>

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

AGESA_STATUS agesa_GetTempHeapBase(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_HeapRebase(UINT32 Func, UINTN Data, VOID *ConfigPtr);

AGESA_STATUS agesa_AllocateBuffer(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_DeallocateBuffer(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_LocateBuffer(UINT32 Func, UINTN Data, VOID *ConfigPtr);

AGESA_STATUS agesa_NoopUnsupported(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_NoopSuccess(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_EmptyIdsInitData(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_Reset(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_RunFuncOnAp(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_GfxGetVbiosImage(UINT32 Func, UINTN FchData,
							VOID *ConfigPrt);

AGESA_STATUS agesa_ReadSpd(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_RunFcnOnAllAps(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_PcieSlotResetControl(UINT32 Func, UINTN Data,
							VOID *ConfigPtr);
AGESA_STATUS agesa_WaitForAllApsFinished(UINT32 Func, UINTN Data,
							VOID *ConfigPtr);
AGESA_STATUS agesa_IdleAnAp(UINT32 Func, UINTN Data, VOID *ConfigPtr);

AGESA_STATUS GetBiosCallout(UINT32 Func, UINTN Data, VOID *ConfigPtr);

AGESA_STATUS agesa_fch_initreset(UINT32 Func, UINTN FchData, VOID *ConfigPtr);
AGESA_STATUS agesa_fch_initenv(UINT32 Func, UINTN FchData, VOID *ConfigPtr);
AGESA_STATUS agesa_HaltThisAp(UINT32 Func, UINTN Data, VOID *ConfigPtr);

void platform_FchParams_reset(FCH_RESET_DATA_BLOCK *FchParams_reset);
void platform_FchParams_env(FCH_DATA_BLOCK *FchParams_env);
void oem_fan_control(FCH_DATA_BLOCK *FchParams);
AGESA_STATUS platform_PcieSlotResetControl(UINT32 Func, UINTN Data,
	VOID *ConfigPtr);
typedef struct {
	UINT32 CalloutName;
	CALLOUT_ENTRY CalloutPtr;
} BIOS_CALLOUT_STRUCT;

extern const BIOS_CALLOUT_STRUCT BiosCallouts[];
extern const int BiosCalloutsLen;

#endif /* __CALLOUTS_AMD_AGESA_H__ */
