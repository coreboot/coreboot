/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <vendorcode/amd/opensil/opensil.h>

#include "opensil.h"

void amd_opensil_silicon_init(void)
{
	setup_opensil();
	opensil_xSIM_timepoint_1();
}

static void call_opensil_xSIM_timepoint_2(void *arg)
{
	opensil_xSIM_timepoint_2();
}

static void call_opensil_xSIM_timepoint_3(void *arg)
{
	opensil_xSIM_timepoint_3();
}

BOOT_STATE_INIT_ENTRY(BS_DEV_RESOURCES, BS_ON_EXIT, call_opensil_xSIM_timepoint_2, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, call_opensil_xSIM_timepoint_3, NULL);
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, call_opensil_xSIM_timepoint_3, NULL);
