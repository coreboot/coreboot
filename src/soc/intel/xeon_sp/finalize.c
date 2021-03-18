/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <console/debug.h>
#include <cpu/x86/smm.h>

static void soc_finalize(void *unused)
{
	printk(BIOS_DEBUG, "Finalizing chipset.\n");

	apm_control(APM_CNT_FINALIZE);

	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_ENTRY, soc_finalize, NULL);
