/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <console/console.h>
#include <device/device.h>
#include <soc/nvidia/tegra/dc.h>
#include <soc/display.h>
#include <soc/sdram.h>
#include <symbols.h>

#include "chip.h"

/* this sucks, but for now, fb size/location are hardcoded.
 * Will break if we get 2. Sigh.
 * We assume it's all multiples of MiB for MMUs sake.
 */
static void soc_read_resources(struct device *dev)
{
	u32 lcdbase = fb_base_mb();
	unsigned long fb_size = FB_SIZE_MB;

	ram_resource_kb(dev, 0, (uintptr_t)_dram/KiB,
		(sdram_max_addressable_mb() - fb_size)*KiB -
		(uintptr_t)_dram/KiB);
	mmio_resource_kb(dev, 1, lcdbase*KiB, fb_size*KiB);

	u32 sdram_end_mb = sdram_size_mb() + (uintptr_t)_dram/MiB;

	if (sdram_end_mb > sdram_max_addressable_mb())
		ram_resource_kb(dev, 2, sdram_max_addressable_mb()*KiB,
			(sdram_end_mb - sdram_max_addressable_mb())*KiB);
}

static void soc_init(struct device *dev)
{
	if (display_init_required())
		display_startup(dev);
	else
		printk(BIOS_INFO, "Skipping display init.\n");
	printk(BIOS_INFO, "CPU: Tegra124\n");
}

static struct device_operations soc_ops = {
	.read_resources   = soc_read_resources,
	.set_resources    = noop_set_resources,
	.init             = soc_init,
};

static void enable_tegra124_dev(struct device *dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_nvidia_tegra124_ops = {
	CHIP_NAME("SOC Nvidia Tegra124")
	.enable_dev = enable_tegra124_dev,
};
