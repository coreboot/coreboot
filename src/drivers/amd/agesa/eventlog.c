/*
 * This file is part of the coreboot project.
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

#include <console/console.h>
#include <stdint.h>
#include <string.h>

#include <northbridge/amd/agesa/state_machine.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include "amdlib.h"
#include "AGESA.h"
#include "AMD.h"

#include <heapManager.h>

static const char undefined[] = "undefined";

/* Match order of enum AGESA_STRUCT_NAME. */
static const char *AgesaFunctionNameStr[] = {
	"AmdInitRecovery", "AmdCreateStruct", "AmdInitEarly", "AmdInitEnv", "AmdInitLate",
	"AmdInitMid", "AmdInitPost", "AmdInitReset", "AmdInitResume", "AmdReleaseStruct",
	"AmdS3LateRestore","AmdS3Save", "AmdGetApicId", "AmdGetPciAddress", "AmdIdentifyCore",
	"AmdReadEventLog", "AmdGetAvailableExeCacheSize", "AmdLateRunApTask", "AmdIdentifyDimm",
};

/* heapManager.h */
static const char *HeapStatusStr[] = {
	"DoNotExistYet", "LocalCache", "TempMem", "SystemMem", "DoNotExistAnymore","S3Resume"
};

/* This function has to match with enumeration of AGESA_STRUCT_NAME defined
 * inside AMD.h header file. Unfortunately those are different across
 * different vendorcode subtrees.
 *
 * TBD: Fix said header or move this function together with the strings above
 * under vendorcode/ tree.
 */

const char *agesa_struct_name(int state)
{
#if IS_ENABLED(CONFIG_CPU_AMD_AGESA_OPENSOURCE)
	if ((state < AMD_INIT_RECOVERY) || (state > AMD_IDENTIFY_DIMMS))
		return undefined;

	int index = state - AMD_INIT_RECOVERY;
#else
	state >>= 12;
	if ((state < AMD_INIT_RECOVERY >> 12) || (state > AMD_IDENTIFY_DIMMS >> 12))
		return undefined;

	int index = state - (AMD_INIT_RECOVERY >> 12);
#endif
	return AgesaFunctionNameStr[index];
}

const char *heap_status_name(int status)
{
	if ((status < HEAP_DO_NOT_EXIST_YET) || (status > HEAP_S3_RESUME))
		return undefined;

	int index = status - HEAP_DO_NOT_EXIST_YET;
	return HeapStatusStr[index];
}

/*
 * Possible AGESA_STATUS values:
 *
 * 0x0 = AGESA_SUCCESS
 * 0x1 = AGESA_UNSUPPORTED
 * 0x2 = AGESA_BOUNDS_CHK
 * 0x3 = AGESA_ALERT
 * 0x4 = AGESA_WARNING
 * 0x5 = AGESA_ERROR
 * 0x6 = AGESA_CRITICAL
 * 0x7 = AGESA_FATAL
 */
static const char *decodeAGESA_STATUS(AGESA_STATUS sret)
{
	const char *statusStrings[] = { "AGESA_SUCCESS", "AGESA_UNSUPPORTED",
					"AGESA_BOUNDS_CHK", "AGESA_ALERT",
					"AGESA_WARNING", "AGESA_ERROR",
					"AGESA_CRITICAL", "AGESA_FATAL"
					};
	if (sret > 7) return "unknown"; /* Non-AGESA error code */
	return statusStrings[sret];
}

static void show_event(EVENT_PARAMS *Event)
{
	printk(BIOS_DEBUG,"\nEventLog:  EventClass = %x, EventInfo = %x.\n",
			(unsigned int)Event->EventClass,
			(unsigned int)Event->EventInfo);
	printk(BIOS_DEBUG,"  Param1 = %x, Param2 = %x.\n",
			(unsigned int)Event->DataParam1,
			(unsigned int)Event->DataParam2);
	printk(BIOS_DEBUG,"  Param3 = %x, Param4 = %x.\n",
			(unsigned int)Event->DataParam3,
			(unsigned int)Event->DataParam4);
}

#define MAX_LOG_ENTRIES 100

static void amd_flush_eventlog(EVENT_PARAMS *Event)
{
	int i = 0;

	do {
		AGESA_STATUS status;
#if HAS_LEGACY_WRAPPER
		status = AmdReadEventLog(Event);
#else
		status = module_dispatch(AMD_READ_EVENT_LOG, &Event->StdHeader);
#endif
		if (status != AGESA_SUCCESS)
			return;
		if (Event->EventClass == 0)
			return;
		show_event(Event);
	} while (++i < MAX_LOG_ENTRIES);
}

void agesawrapper_trace(AGESA_STATUS ret, AMD_CONFIG_PARAMS *StdHeader,
	const char *func)
{
	EVENT_PARAMS AmdEventParams;

	printk(BIOS_DEBUG, "%s() returned %s\n", func, decodeAGESA_STATUS(ret));
	if (ret == AGESA_SUCCESS)
		return;

	memset(&AmdEventParams, 0, sizeof(EVENT_PARAMS));

	if (HAS_LEGACY_WRAPPER) {
		AmdEventParams.StdHeader.AltImageBasePtr = 0;
		AmdEventParams.StdHeader.CalloutPtr = &GetBiosCallout;
		AmdEventParams.StdHeader.Func = 0;
		AmdEventParams.StdHeader.ImageBasePtr = 0;
		AmdEventParams.StdHeader.HeapStatus = StdHeader->HeapStatus;
	} else {
		memcpy(&AmdEventParams.StdHeader, StdHeader, sizeof(*StdHeader));
	}

	amd_flush_eventlog(&AmdEventParams);
}

AGESA_STATUS agesawrapper_amdreadeventlog (UINT8 HeapStatus)
{
	EVENT_PARAMS AmdEventParams;

	memset(&AmdEventParams, 0, sizeof(EVENT_PARAMS));

	AmdEventParams.StdHeader.AltImageBasePtr = 0;
	AmdEventParams.StdHeader.CalloutPtr = &GetBiosCallout;
	AmdEventParams.StdHeader.Func = 0;
	AmdEventParams.StdHeader.ImageBasePtr = 0;
	AmdEventParams.StdHeader.HeapStatus = HeapStatus;

	amd_flush_eventlog(&AmdEventParams);

	return AGESA_SUCCESS;
}
