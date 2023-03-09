/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <types.h>
#include "psp_def.h"

static void psp_set_spl_fuse(void *unused)
{
	int cmd_status = 0;
	struct mbox_cmd_late_spl_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		}
	};
	uint32_t c2p38 = soc_read_c2p38();

	if (c2p38 & CORE_2_PSP_MSG_38_FUSE_SPL) {
		printk(BIOS_DEBUG, "PSP: SPL Fusing may be updated.\n");
	} else {
		printk(BIOS_DEBUG, "PSP: SPL Fusing not currently required.\n");
		return;
	}

	if (c2p38 & CORE_2_PSP_MSG_38_SPL_FUSE_ERROR) {
		printk(BIOS_ERR, "PSP: SPL Table does not meet fuse requirements.\n");
		return;
	}

	if (c2p38 & CORE_2_PSP_MSG_38_SPL_ENTRY_ERROR) {
		printk(BIOS_ERR, "PSP: Critical SPL entry missing or current firmware does"
				   " not meet requirements.\n");
		return;
	}

	if (c2p38 & CORE_2_PSP_MSG_38_SPL_ENTRY_MISSING) {
		printk(BIOS_ERR, "PSP: Table of critical SPL values is missing.\n");
		return;
	}

	if (!CONFIG(SOC_AMD_COMMON_BLOCK_PSP_FUSE_SPL))
		return;

	printk(BIOS_DEBUG, "PSP: SPL Fusing Update Requested.\n");
	cmd_status = send_psp_command(MBOX_BIOS_CMD_SET_SPL_FUSE, &buffer);
	psp_print_cmd_status(cmd_status, NULL);
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_ENTRY, psp_set_spl_fuse, NULL);
