/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CALLOUTS_AMD_AGESA_H
#define CALLOUTS_AMD_AGESA_H

#include <Porting.h>
#include <AGESA.h>

AGESA_STATUS agesa_NoopUnsupported(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_NoopSuccess(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_EmptyIdsInitData(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_Reset(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_RunFuncOnAp(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_GfxGetVbiosImage(UINT32 Func, UINTN FchData, VOID *ConfigPrt);

AGESA_STATUS agesa_ReadSpd(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS agesa_ReadSpd_from_cbfs(UINT32 Func, UINTN Data, VOID *ConfigPtr);

AGESA_STATUS HeapManagerCallout(UINT32 Func, UINTN Data, VOID *ConfigPtr);
AGESA_STATUS GetBiosCallout(UINT32 Func, UINTN Data, VOID *ConfigPtr);

typedef struct {
	UINT32 CalloutName;
	CALLOUT_ENTRY CalloutPtr;
} BIOS_CALLOUT_STRUCT;

extern const BIOS_CALLOUT_STRUCT BiosCallouts[];
extern const int BiosCalloutsLen;

#endif /* CALLOUTS_AMD_AGESA_H */
