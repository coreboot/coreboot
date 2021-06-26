/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <console/console.h>
#include <device/device.h>
#include <gpio.h>
#include <soc/display.h>
#include <soc/soc.h>
#include <soc/sdram.h>
#include <symbols.h>

#include "chip.h"

static void soc_init(struct device *dev)
{
	ram_range(dev, 0, (uintptr_t)_dram, sdram_size_mb() * MiB);
	if (display_init_required())
		rk_display_init(dev, (uintptr_t)_framebuffer,
				REGION_SIZE(framebuffer));
	else
		printk(BIOS_INFO, "Skipping display init.\n");
}

static struct device_operations soc_ops = {
	.read_resources   = noop_read_resources,
	.set_resources    = noop_set_resources,
	.init             = soc_init,
};

static void enable_rk3288_dev(struct device *dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_rockchip_rk3288_ops = {
	CHIP_NAME("SOC Rockchip 3288")
	.enable_dev = enable_rk3288_dev,
};
