/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <console/console.h>
#include <soc/mmu.h>
#include <soc/shrm.h>
#include <soc/clock.h>

void shrm_fw_load_reset(void)
{
	bool shrm_fw_entry;
	struct prog shrm_fw_prog =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/shrm");

	shrm_fw_entry = selfload(&shrm_fw_prog);
	if (!shrm_fw_entry)
		die("SOC image: SHRM load failed");

	clock_reset_shrm();

	printk(BIOS_DEBUG, "\nSOC:SHRM brought out of reset.\n");
}
