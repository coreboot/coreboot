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
	uint64_t lcdbase = fb_base_mb();
	uint64_t fb_size = FB_SIZE_MB;

	ram_from_to(dev, 0, (uintptr_t)_dram, (sdram_max_addressable_mb() - fb_size) * MiB);
	mmio_range(dev, 1, lcdbase * MiB, fb_size * MiB);

	ram_from_to(dev, 2, sdram_max_addressable_mb() * (uint64_t)MiB,
		    (uintptr_t)_dram + sdram_size_mb() * (uint64_t)MiB);
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
