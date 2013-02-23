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

#ifndef CALLOUTS_AMD_AGESA_FAM15TN_H
#define CALLOUTS_AMD_AGESA_FAM15TN_H

#include "Porting.h"
#include "AGESA.h"

#define BIOS_HEAP_START_ADDRESS  0x010000000
#define BIOS_HEAP_SIZE				0x30000
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

AGESA_STATUS fam15tn_AllocateBuffer (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS fam15tn_DeallocateBuffer (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS fam15tn_LocateBuffer (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS fam15tn_GetIdsInitData (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS fam15tn_Reset (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS fam15tn_RunFuncOnAp (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS fam15tn_HookBeforeDQSTraining (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS fam15tn_HookBeforeExitSelfRefresh (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS fam15tn_DefaultRet (UINT32 Func, UINT32 Data, VOID *ConfigPtr);
AGESA_STATUS fam15tn_HookGfxGetVbiosImage(UINT32 Func, UINT32 FchData, VOID *ConfigPrt);
AGESA_STATUS fam15tn_ReadSpd (UINT32 Func, UINT32 Data, VOID *ConfigPtr);

#endif /* CALLOUTS_AMD_AGESA_FAM15TN_H */
