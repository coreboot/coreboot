/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <stdint.h>
#include <string.h>
#include <timestamp.h>

#include <northbridge/amd/agesa/state_machine.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <amdlib.h>
#include <debug_util.h>
#include <AGESA.h>
#include <AMD.h>

static const char undefined[] = "undefined";

struct agesa_mapping
{
	AGESA_STRUCT_NAME func;
	const char *name;
	uint32_t entry_id;
	uint32_t exit_id;
};

static const struct agesa_mapping entrypoint[] = {
	{
		.func = AMD_INIT_RESET,
		.name = "AmdInitReset",
		.entry_id = TS_AGESA_INIT_RESET_START,
		.exit_id = TS_AGESA_INIT_RESET_END,
	},
	{
		.func = AMD_INIT_EARLY,
		.name = "AmdInitEarly",
		.entry_id = TS_AGESA_INIT_EARLY_START,
		.exit_id = TS_AGESA_INIT_EARLY_END,
	},
	{
		.func = AMD_INIT_POST,
		.name = "AmdInitPost",
		.entry_id = TS_AGESA_INIT_POST_START,
		.exit_id = TS_AGESA_INIT_POST_END,
	},
	{
		.func = AMD_INIT_RESUME,
		.name = "AmdInitResume",
		.entry_id = TS_AGESA_INIT_RESUME_START,
		.exit_id = TS_AGESA_INIT_RESUME_END,
	},
	{
		.func = AMD_INIT_ENV,
		.name = "AmdInitEnv",
		.entry_id = TS_AGESA_INIT_ENV_START,
		.exit_id = TS_AGESA_INIT_ENV_END,
	},
	{
		.func = AMD_INIT_MID,
		.name = "AmdInitMid",
		.entry_id = TS_AGESA_INIT_MID_START,
		.exit_id = TS_AGESA_INIT_MID_END,
	},
	{
		.func = AMD_INIT_LATE,
		.name = "AmdInitLate",
		.entry_id = TS_AGESA_INIT_LATE_START,
		.exit_id = TS_AGESA_INIT_LATE_END,
	},
	{
		.func = AMD_S3LATE_RESTORE,
		.name = "AmdS3LateRestore",
		.entry_id = TS_AGESA_S3_LATE_START,
		.exit_id = TS_AGESA_S3_LATE_END,
	},
#if !defined(AMD_S3_SAVE_REMOVED)
	{
		.func = AMD_S3_SAVE,
		.name = "AmdS3Save",
		.entry_id = TS_AGESA_INIT_RTB_START,
		.exit_id = TS_AGESA_INIT_RTB_END,
	},
#endif
	{
		.func = AMD_S3FINAL_RESTORE,
		.name = "AmdS3FinalRestore",
		.entry_id = TS_AGESA_S3_FINAL_START,
		.exit_id = TS_AGESA_S3_FINAL_END,
	},
	{
		.func = AMD_INIT_RTB,
		.name = "AmdInitRtb",
		.entry_id = TS_AGESA_INIT_RTB_START,
		.exit_id = TS_AGESA_INIT_RTB_END,
	},
};

void agesa_state_on_entry(struct agesa_state *task, AGESA_STRUCT_NAME func)
{
	int i;

	task->apic_id = (u8)initial_lapicid();
	task->func = func;
	task->function_name = undefined;

	for (i = 0; i < ARRAY_SIZE(entrypoint); i++) {
		if (task->func == entrypoint[i].func) {
			task->function_name = entrypoint[i].name;
			task->ts_entry_id = entrypoint[i].entry_id;
			task->ts_exit_id = entrypoint[i].exit_id;
			break;
		}
	}

	printk(BIOS_DEBUG, "\nAPIC %02d: ** Enter %s [%08x]\n",
		task->apic_id, task->function_name, task->func);
}

void agesa_state_on_exit(struct agesa_state *task,
	AMD_CONFIG_PARAMS *StdHeader)
{
	printk(BIOS_DEBUG, "APIC %02d: Heap in %s (%d) at 0x%08x\n",
		task->apic_id, heap_status_name(StdHeader->HeapStatus),
		StdHeader->HeapStatus, (u32)StdHeader->HeapBasePtr);

	printk(BIOS_DEBUG, "APIC %02d: ** Exit  %s [%08x]\n",
		task->apic_id, task->function_name, task->func);
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
		status = module_dispatch(AMD_READ_EVENT_LOG, &Event->StdHeader);
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
	memcpy(&AmdEventParams.StdHeader, StdHeader, sizeof(*StdHeader));

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
