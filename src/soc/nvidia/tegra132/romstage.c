/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/stages.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/cbmem_console.h>
#include <console/console.h>
#include <arch/exception.h>

#include <soc/addressmap.h>
#include <soc/sdram_configs.h>
#include "sdram.h"
#include "ccplex.h"

void romstage(void);
void romstage(void)
{
	void *entry;

	console_init();
	exception_init();

	printk(BIOS_INFO, "T132: romstage here\n");

#if CONFIG_BOOTROM_SDRAM_INIT
	printk(BIOS_INFO, "T132 romstage: SDRAM init done by BootROM, RAMCODE = %d\n",
		sdram_get_ram_code());
#else
	sdram_init(get_sdram_config());
	printk(BIOS_INFO, "T132 romstage: sdram_init done\n");
#endif

	/*
	 * Trust Zone needs to be initialized after the DRAM initialization
	 * because carveout registers are programmed during DRAM init.
	 * cbmem_initialize() is dependent on the Trust Zone region
	 * initalization because CBMEM lives right below the Trust Zone which
	 * needs to be properly identified.
	 */
	trustzone_region_init();

	cbmem_initialize();

	ccplex_cpu_prepare();
	printk(BIOS_INFO, "T132 romstage: cpu prepare done\n");

	ccplex_load_mts();
	printk(BIOS_INFO, "T132 romstage: MTS loading done\n");

	entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA,
				CONFIG_CBFS_PREFIX "/ramstage");

	cbmemc_reinit();

	ccplex_cpu_start(entry);

	while (1);
}
