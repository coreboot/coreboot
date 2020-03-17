/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <bootmem.h>
#include <bootmode.h>
#include <console/console.h>
#include <device/device.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/display.h>
#include <soc/sdram.h>
#include <soc/symbols.h>
#include <stddef.h>
#include <symbols.h>

void bootmem_platform_add_ranges(void)
{
	bootmem_add_range((uintptr_t)_pmu_sram, REGION_SIZE(pmu_sram),
			  BM_MEM_BL31);
	bootmem_add_range((uintptr_t)_bl31_sram, REGION_SIZE(bl31_sram),
			  BM_MEM_BL31);
}

static void soc_read_resources(struct device *dev)
{
	ram_resource(dev, 0, (uintptr_t)_dram / KiB, sdram_size_mb() * KiB);
}

static void soc_init(struct device *dev)
{
	if (CONFIG(MAINBOARD_DO_NATIVE_VGA_INIT) && display_init_required())
		rk_display_init(dev);
	else
		printk(BIOS_INFO, "Display initialization disabled.\n");

	/* We don't need big CPUs, but bring them up as a courtesy to Linux. */
	rkclk_configure_cpu(APLL_600_MHZ, CPU_CLUSTER_BIG);
}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
	.init = soc_init,
};

static void enable_soc_dev(struct device *dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_rockchip_rk3399_ops = {
	CHIP_NAME("SOC Rockchip RK3399")
	    .enable_dev = enable_soc_dev,
};
