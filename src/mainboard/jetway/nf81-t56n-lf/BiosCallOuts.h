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

#include <stdint.h>

#define BIOS_HEAP_START_ADDRESS			0x10000 /* HEAP during cold boot */
#define BIOS_HEAP_SIZE				0x20000
#define BSP_STACK_BASE_ADDR			0x30000

typedef struct _BIOS_HEAP_MANAGER {
	//uint32_t AvailableSize;
	uint32_t StartOfAllocatedNodes;
	uint32_t StartOfFreedNodes;
} BIOS_HEAP_MANAGER;

typedef struct _BIOS_BUFFER_NODE {
	uint32_t BufferHandle;
	uint32_t BufferSize;
	uint32_t NextNodeOffset;
} BIOS_BUFFER_NODE;
/*
 * CALLOUTS
 */
AGESA_STATUS GetBiosCallout (uint32_t Func, uint32_t Data, void *ConfigPtr);

/* REQUIRED CALLOUTS
 * AGESA ADVANCED CALLOUTS - CPU
 */
AGESA_STATUS BiosAllocateBuffer (uint32_t Func, uint32_t Data, void *ConfigPtr);
AGESA_STATUS BiosDeallocateBuffer (uint32_t Func, uint32_t Data, void *ConfigPtr);
AGESA_STATUS BiosLocateBuffer (uint32_t Func, uint32_t Data, void *ConfigPtr);
AGESA_STATUS BiosRunFuncOnAp (uint32_t Func, uint32_t Data, void *ConfigPtr);
AGESA_STATUS BiosReset (uint32_t Func, uint32_t Data, void *ConfigPtr);
AGESA_STATUS BiosGetIdsInitData (uint32_t Func, uint32_t Data, void *ConfigPtr);

/* AGESA Advanced Callouts - Memory */
AGESA_STATUS BiosReadSpd (uint32_t Func,uint32_t	Data,void *ConfigPtr);

/* BIOS default RET */
AGESA_STATUS BiosDefaultRet (uint32_t Func, uint32_t Data, void *ConfigPtr);

/* Call the host environment interface to provide a user hook opportunity. */
AGESA_STATUS BiosHookBeforeDQSTraining (uint32_t Func, uint32_t Data, void *ConfigPtr);
/* Call the host environment interface to provide a user hook opportunity. */
AGESA_STATUS BiosHookBeforeDramInit (uint32_t Func, uint32_t Data, void *ConfigPtr);
/* Call the host environment interface to provide a user hook opportunity. */
AGESA_STATUS BiosHookBeforeDramInitRecovery (uint32_t Func, uint32_t Data, void *ConfigPtr);
/* Call the host environment interface to provide a user hook opportunity. */
AGESA_STATUS BiosHookBeforeExitSelfRefresh (uint32_t Func, uint32_t Data, void *ConfigPtr);
/* PCIE slot reset control */
AGESA_STATUS BiosGnbPcieSlotReset (uint32_t Func, uint32_t Data, void *ConfigPtr);

#endif //_BIOS_CALLOUT_H_
