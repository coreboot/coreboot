/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/romstage.h>

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	 /* TODO: Placeholder for overriding FSP-M UPDs */
}

__weak void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
