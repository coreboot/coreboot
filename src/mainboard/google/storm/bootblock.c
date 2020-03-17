/*
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <bootblock_common.h>
#include <console/console.h>
#include <symbols.h>

#include "mmu.h"
#include <soc/usbl_if.h>

static void dump_usbl_report(int section, sbl_ro_info *info)
{
	int i, num_log_entries;

	num_log_entries = info->num_log_entries;
	if (!num_log_entries)
		return;

	printk(BIOS_INFO, "  Section %d log:\n", section);
	for (i = 0; i < num_log_entries; i++)
		printk(BIOS_INFO, "    %-5d:%2.2x:%.*s\n",
		       info->log[i].time_stamp,
		       info->log[i].type,
		       sizeof(info->log[i].msg),
		       info->log[i].msg);
}

void bootblock_mainboard_init(void)
{
	int i;

	setup_mmu(DRAM_NOT_INITIALIZED);

	if (((uintptr_t)maskrom_param < (uintptr_t)&_sram) ||
	    ((uintptr_t)maskrom_param > (uintptr_t)&_esram)) {
		printk(BIOS_INFO, "No uber-sbl parameter detected\n");
		return;
	}

	/* Is maskrom parameter address set to a sensible value? */
	if ((maskrom_param->start_magic != UBER_SBL_SHARED_INFO_START_MAGIC) ||
	    (maskrom_param->end_magic != UBER_SBL_SHARED_INFO_END_MAGIC)) {

		printk(BIOS_INFO, "Uber-sbl: invalid magic!\n");
	} else {
		printk(BIOS_INFO, "Uber-sbl version: %s\n",
		       maskrom_param->version);

		for (i = 0; i < maskrom_param->num; i++)
			dump_usbl_report(i, &maskrom_param->info[i]);
	}
}
