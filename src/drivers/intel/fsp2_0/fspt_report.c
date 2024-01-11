/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/symbols.h>
#include <console/console.h>
#include <fsp/util.h>

/* filled in assembly after FSP-T ran */
uintptr_t temp_memory_start;
uintptr_t temp_memory_end;

void report_fspt_output(void)
{
	const struct region fsp_car_region = region_create(
			temp_memory_start, temp_memory_end - temp_memory_start);
	const struct region coreboot_car_region = region_create(
			(uintptr_t)_car_region_start, (uintptr_t)_car_region_size);

	printk(BIOS_DEBUG, "FSP-T: reported temp_mem region: [0x%08lx,0x%08lx)\n",
	       temp_memory_start, temp_memory_end);
	if (!region_is_subregion(&fsp_car_region, &coreboot_car_region)) {
		printk(BIOS_ERR, "Wrong CAR region used!\n");
		printk(BIOS_ERR, "Adapt CONFIG_DCACHE_RAM_BASE and CONFIG_DCACHE_RAM_SIZE to match FSP-T\n");
	}
}
