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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <delay.h>
#include <console/console.h>
#include <device/device.h>
#include <cbmem.h>
#include <arch/cache.h>
#include "dp.h"
#include "fimd.h"
#include "cpu.h"
#include "clk.h"
#include "usb.h"
#include "chip.h"

static unsigned int cpu_id;
static unsigned int cpu_rev;

/* Setting TZPC[TrustZone Protection Controller]
 * We pretty much disable it all, as the kernel
 * expects it that way -- and that's not the default.
 */
static void tzpc_init(void)
{
	struct exynos_tzpc *tzpc;
	unsigned int addr;

	for (addr = TZPC10_BASE; addr <= TZPC9_BASE; addr += TZPC_BASE_OFFSET) {
		tzpc = (struct exynos_tzpc *)addr;
		if (addr == TZPC0_BASE)
			writel(R0SIZE, &tzpc->r0size);
		writel(DECPROTXSET, &tzpc->decprot0set);
		writel(DECPROTXSET, &tzpc->decprot1set);
		writel(DECPROTXSET, &tzpc->decprot2set);
		writel(DECPROTXSET, &tzpc->decprot3set);
	}
}

static void set_cpu_id(void)
{
	u32 pro_id = (read32((void *)EXYNOS_PRO_ID) & 0x00FFF000) >> 12;

	switch (pro_id) {
	case 0x200:
		/* Exynos4210 EVT0 */
		cpu_id = 0x4210;
		cpu_rev = 0;
		break;
	case 0x210:
		/* Exynos4210 EVT1 */
		cpu_id = 0x4210;
		break;
	case 0x412:
		/* Exynos4412 */
		cpu_id = 0x4412;
		break;
	case 0x520:
		/* Exynos5250 */
		cpu_id = 0x5250;
		break;
	case 0x420:
		/* Exynos5420 */
		cpu_id = 0x5420;
		break;
	}
}

/* we distinguish a display port device from a raw graphics device
 * because there are dramatic differences in startup depending on
 * graphics usage. To make startup fast and easier to understand and
 * debug we explicitly name this common case. The alternate approach,
 * involving lots of machine and callbacks, is hard to debug and
 * verify.
 */
static void exynos_displayport_init(device_t dev, u32 lcdbase,
		unsigned long fb_size)
{
	struct cpu_samsung_exynos5420_config *conf = dev->chip_info;
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
	 * stuff inadvertantly.
	 */
	uint32_t lower = ALIGN_DOWN(lcdbase, MiB);
	uint32_t upper = ALIGN_UP(lcdbase + fb_size, MiB);

	dcache_clean_invalidate_by_mva(lower, upper - lower);
	mmu_config_range(lower / MiB, (upper - lower) / MiB, DCACHE_OFF);

	mmio_resource(dev, 1, lcdbase/KiB, (fb_size + KiB - 1)/KiB);
}

static void cpu_enable(device_t dev)
{
	unsigned long fb_size = FB_SIZE_KB * KiB;
	u32 lcdbase = get_fb_base_kb() * KiB;

	ram_resource(dev, 0, RAM_BASE_KB, RAM_SIZE_KB - FB_SIZE_KB);
	mmio_resource(dev, 1, lcdbase / KiB, (fb_size + KiB - 1) / KiB);

	exynos_displayport_init(dev, lcdbase, fb_size);

	set_cpu_id();

	tzpc_init();
}

static void cpu_init(device_t dev)
{
	printk(BIOS_INFO, "CPU:   S5P%X @ %ldMHz\n",
			cpu_id, get_arm_clk() / 1000000);

	usb_init(dev);
}

static void cpu_noop(device_t dev)
{
}

static struct device_operations cpu_ops = {
	.read_resources   = cpu_noop,
	.set_resources    = cpu_noop,
	.enable_resources = cpu_enable,
	.init             = cpu_init,
	.scan_bus         = 0,
};

static void enable_exynos5420_dev(device_t dev)
{
	dev->ops = &cpu_ops;
}

struct chip_operations cpu_samsung_exynos5420_ops = {
	CHIP_NAME("CPU Samsung Exynos 5420")
	.enable_dev = enable_exynos5420_dev,
};

void exynos5420_config_l2_cache(void)
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

	val = read_l2actlr();

	/* L2ACTLR[3]: Disable clean/evict push to external */
	val |= (1 << 3);

	/* L2ACTLR[7]: Enable hazard detect timeout for A15 */
	val |= (1 << 7);

	/* L2ACTLR[27]: Prevents stopping the L2 logic clock */
	val |= (1 << 27);

	write_l2actlr(val);

	/* Read the l2 control register to force things to take effect? */
	val = read_l2ctlr();
}
