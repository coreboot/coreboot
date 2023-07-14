/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/reset.h>
#include <bootstate.h>
#include <cbmem.h>
#include <xSIM-api.h>
#include "opensil_console.h"

static void SIL_STATUS_report(const char *function, const int status)
{
	const int log_level = status == SilPass ? BIOS_DEBUG : BIOS_ERR;
	const char *error_string = "Unkown error";

	const struct error_string_entry {
		SIL_STATUS status;
		const char *string;
	} errors[] = {
		{SilPass, "SilPass"},
		{SilUnsupportedHardware, "SilUnsupportedHardware"},
		{SilUnsupported, "SilUnsupported"},
		{SilInvalidParameter, "SilInvalidParameter"},
		{SilAborted, "SilAborted"},
		{SilOutOfResources, "SilOutOfResources"},
		{SilNotFound, "SilNotFound"},
		{SilOutOfBounds, "SilOutOfBounds"},
		{SilDeviceError, "SilDeviceError"},
		{SilResetRequestColdImm, "SilResetRequestColdImm"},
		{SilResetRequestColdDef, "SilResetRequestColdDef"},
		{SilResetRequestWarmImm, "SilResetRequestWarmImm"},
		{SilResetRequestWarmDef, "SilResetRequestWarmDef"},
	};

	int i;
	for (i = 0; i < ARRAY_SIZE(errors); i++) {
		if (errors[i].status == status)
			error_string = errors[i].string;
	}
	printk(log_level, "%s returned %d (%s)\n", function, status, error_string);
}

static void setup_opensil(void *unused)
{
	const SIL_STATUS debug_ret = SilDebugSetup(HostDebugService);
	SIL_STATUS_report("SilDebugSetup", debug_ret);
	const size_t mem_req = xSimQueryMemoryRequirements();
	void *buf = cbmem_add(CBMEM_ID_AMD_OPENSIL, mem_req);
	assert(buf);
	/* We run all openSIL timepoints in the same stage so using TP1 as argument is fine. */
	const SIL_STATUS assign_mem_ret = xSimAssignMemoryTp1(buf, mem_req);
	SIL_STATUS_report("xSimAssignMemory", assign_mem_ret);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT_CHIPS, BS_ON_ENTRY, setup_opensil, NULL);

static void opensil_entry(void *timepoint)
{
	SIL_STATUS ret;
	SIL_TIMEPOINT tp = (uintptr_t)timepoint;

	switch (tp) {
	case SIL_TP1:
		ret = InitializeSiTp1();
		break;
	case SIL_TP2:
		ret = InitializeSiTp2();
		break;
	case SIL_TP3:
		ret = InitializeSiTp3();
		break;
	default:
		printk(BIOS_ERR, "Unknown opensil timepoint\n");
		return;
	}
	char opensil_function[16];
	snprintf(opensil_function, sizeof(opensil_function), "InitializeSiTp%d", tp);
	SIL_STATUS_report(opensil_function, ret);
	if (ret == SilResetRequestColdImm || ret == SilResetRequestColdDef) {
		printk(BIOS_INFO, "openSil requested a cold reset");
		do_cold_reset();
	} else if (ret == SilResetRequestWarmImm || ret == SilResetRequestWarmDef) {
		printk(BIOS_INFO, "openSil requested a warm reset");
		do_warm_reset();
	}
}

/* TODO: look into calling these functions from some SoC device operations instead of using
 * BOOT_STATE_INIT_ENTRY */
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT_CHIPS, BS_ON_EXIT, opensil_entry, (void *)SIL_TP1);
/* TODO add other timepoints later. Are they NOOP? */
