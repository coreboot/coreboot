/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <bootstate.h>
#include <cbmem.h>

static void add_pvmfw_cbmem(void *unused)
{
	(void)unused;
	void *pvmfw;

	pvmfw = cbmem_add(CBMEM_ID_PVMFW, CONFIG_GOOGLE_PVMFW_CBMEM_SIZE);
	if (!pvmfw)
		printk(BIOS_ERR, "Failed to add pvmfw info to CBMEM\n");
}

BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY, add_pvmfw_cbmem, NULL);
