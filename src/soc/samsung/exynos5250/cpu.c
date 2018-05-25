/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 * Copyright (C) 2012 Samsung Electronics
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

#include <arch/cache.h>
#include <cbmem.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <soc/clk.h>
#include <soc/cpu.h>
#include <soc/dp-core.h>
#include <soc/fimd.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "chip.h"

static unsigned int cpu_id;
static unsigned int cpu_rev;

static void set_cpu_id(void)
{
	cpu_id = read32((void *)EXYNOS5_PRO_ID);
	cpu_id = (0xC000 | ((cpu_id & 0x00FFF000) >> 12));

	/*
	 * 0xC200: EXYNOS4210 EVT0
	 * 0xC210: EXYNOS4210 EVT1
	 */
	if (cpu_id == 0xC200) {
		cpu_id |= 0x10;
		cpu_rev = 0;
	} else if (cpu_id == 0xC210) {
		cpu_rev = 1;
	}
}

/* we distinguish a display port device from a raw graphics device
 * because there are dramatic differences in startup depending on
 * graphics usage. To make startup fast and easier to understand and
 * debug we explicitly name this common case. The alternate approach,
 * involving lots of machine and callbacks, is hard to debug and
 * verify.
 */
static void exynos_displayport_init(struct device *dev, u32 lcdbase,
		unsigned long fb_size)
{
	struct soc_samsung_exynos5250_config *conf = dev->chip_info;
	/* put these on the stack. If, at some point, we want to move
	 * this code to a pre-ram stage, it will be much easier.
	 */
	struct exynos5_fimd_panel panel;
	memset(&panel, 0, sizeof(panel));

	panel.is_dp = 1; /* Display I/F is eDP */
	/* while it is true that we did a memset to zero,
	 * we leave some 'set to zero' entries here to make
	 * it clear what's going on. Graphics is confusing.
	 */
	panel.is_mipi = 0;
	panel.fixvclk = 0;
	panel.ivclk = 0;
	panel.clkval_f = conf->clkval_f;
	panel.upper_margin = conf->upper_margin;
	panel.lower_margin = conf->lower_margin;
	panel.vsync = conf->vsync;
	panel.left_margin = conf->left_margin;
	panel.right_margin = conf->right_margin;
	panel.hsync = conf->hsync;
	panel.xres = conf->xres;
	panel.yres = conf->yres;

	printk(BIOS_SPEW, "LCD framebuffer @%p\n", (void *)(lcdbase));
	memset((void *)lcdbase, 0, fb_size);	/* clear the framebuffer */

	/*
	 * We need to clean and invalidate the framebuffer region and disable
	 * caching as well. We assume that our dcache <--> memory address
	 * space is identity-mapped in 1MB chunks, so align accordingly.
	 *
	 * Note: We may want to do something clever to ensure the framebuffer
	 * region is aligned such that we don't change dcache policy for other
	 * stuff inadvertently.
	 */
	uint32_t lower = ALIGN_DOWN(lcdbase, MiB);
	uint32_t upper = ALIGN_UP(lcdbase + fb_size, MiB);

	dcache_clean_invalidate_by_mva((void *)lower, upper - lower);
	mmu_config_range(lower / MiB, (upper - lower) / MiB, DCACHE_OFF);

	printk(BIOS_DEBUG, "Initializing Exynos LCD.\n");

	lcd_ctrl_init(fb_size, &panel, (void *)lcdbase);
}

static void cpu_enable(struct device *dev)
{
	unsigned long fb_size = FB_SIZE_KB * KiB;
	u32 lcdbase = get_fb_base_kb() * KiB;

	ram_resource(dev, 0, RAM_BASE_KB, RAM_SIZE_KB - FB_SIZE_KB);
	mmio_resource(dev, 1, lcdbase / KiB, CEIL_DIV(fb_size, KiB));

	exynos_displayport_init(dev, lcdbase, fb_size);

	set_cpu_id();
}

static void cpu_init(struct device *dev)
{
	printk(BIOS_INFO, "CPU:   S5P%X @ %ldMHz\n",
			cpu_id, get_arm_clk() / (1024*1024));
}

static struct device_operations cpu_ops = {
	.read_resources   = DEVICE_NOOP,
	.set_resources    = DEVICE_NOOP,
	.enable_resources = cpu_enable,
	.init             = cpu_init,
	.scan_bus         = 0,
};

static void enable_exynos5250_dev(struct device *dev)
{
	dev->ops = &cpu_ops;
}

struct chip_operations soc_samsung_exynos5250_ops = {
	CHIP_NAME("SOC Samsung Exynos 5250")
	.enable_dev = enable_exynos5250_dev,
};

void exynos5250_config_l2_cache(void)
{
	uint32_t val;

	/*
	 * Bit    9 - L2 tag RAM setup (1 cycle)
	 * Bits 8:6 - L2 tag RAM latency (3 cycles)
	 * Bit    5 - L2 data RAM setup (1 cycle)
	 * Bits 2:0 - L2 data RAM latency (3 cycles)
	 */
	val = (1 << 9) | (0x2 << 6) | (1 << 5) | (0x2);
	write_l2ctlr(val);
}
