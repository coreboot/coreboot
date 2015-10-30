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

#ifndef CALLOUTS_AMD_AGESA_H
#define CALLOUTS_AMD_AGESA_H

#include "Porting.h"
#include "AGESA.h"

AGESA_STATUS agesa_NoopUnsupported (UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_NoopSuccess (UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_EmptyIdsInitData (UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_Reset (UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_RunFuncOnAp (UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_GfxGetVbiosImage(UINT32 Func, UINTN FchData, VOID *ConfigPrt);

AGESA_STATUS agesa_ReadSpd (UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_ReadSpd_from_cbfs(UINT32 Func, UINTN Data, VOID *ConfigPtr);

AGESA_STATUS HeapManagerCallout (UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS GetBiosCallout (UINT32 Func, UINTN Data, VOID *ConfigPtr);

typedef struct {
	UINT32 CalloutName;
	CALLOUT_ENTRY CalloutPtr;
} BIOS_CALLOUT_STRUCT;

extern const BIOS_CALLOUT_STRUCT BiosCallouts[];
extern const int BiosCalloutsLen;

#endif /* CALLOUTS_AMD_AGESA_H */
