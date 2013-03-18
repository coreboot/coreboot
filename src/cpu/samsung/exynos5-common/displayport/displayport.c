/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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
#include <arch/io.h>
#include <device/device.h>

/* we distinguish a display port device from a raw graphics device because there are
 * dramatic differences in startup depending on graphics usage. To make startup fast
 * and easier to understand and debug we explicitly name this common case. The alternate
 * approach, involving lots of machine and callbacks, is hard to debug and verify.
 */
static void exynos_displayport_init(device_t dev)
{
	struct cpu_samsung_exynos5_common_displayport_config *conf = dev->chip_info;
	/* put these on the stack. If, at some point, we want to move this code to a
	 * pre-ram stage, it will be much easier.
	 */
	vidinfo_t vi;
	struct exynos5_fimd_panel panel;
	void *lcdbase;

	memset(vi, 0, sizeof(vi));
	memset(panel, 0, sizeof(panel));

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

	vi->vl_col = conf->xres;
	vi->fl_row = conf->yres;
	vi->vl_bpix = conf->bpp;
	vi->cmap = cbmem_reserve(64*1024); /* The size is a magic number from hardware. */

	lcdbase = conf->lcdbase;
	printk(BIOS_DEBUG, "Initializing exynos VGA\n");
	ret = lcd_ctrl_init(&vi, &panel, lcdbase);
#if 0
	ret = board_dp_lcd_vdd(blob, &wait_ms);
	ret = board_dp_bridge_setup(blob, &wait_ms);
	while (tries < 5) {
		ret = board_dp_bridge_init(blob, &wait_ms);
		ret = board_dp_hotplug(blob, &wait_ms);
		if (ret) {
			ret = board_dp_bridge_reset(blob, &wait_ms);
			continue;
		}
		ret = dp_controller_init(blob, &wait_ms);
		ret = board_dp_backlight_vdd(blob, &wait_ms);
		ret = board_dp_backlight_pwm(blob, &wait_ms);
		ret = board_dp_backlight_en(blob, &wait_ms);
	}
#endif
}

static void exynos_displayport_read_resources(device_t dev)
{
	struct cpu_samsung_exynos5_common_displayport_config *conf = dev->chip_info;
        struct resource *resource;
	printk(BIOS_SPEW, "%s: dev %p\n", __func__, dev);
	exynos_displayport_init(dev);
	/* claim a resource for the UMA graphics.
	 * Follow the current convention of starting at 24M
	 * from the start.
	 */
	resource = new_resource(dev, 0);
	/* this is a hardcode for now. There's some real confusion about what it
	 * needs to be, docs are not helping, and hardware on real systems
	 * has settings we don't understand. FIXME.
	 */
	resource->base = 0x20000000 + 24*MiB;
	resource->size = conf->xres * conf->yres * 4; /* 4 bytes per pixel for RGB */
	resource->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
	printk(BIOS_DEBUG, "Adding graphics at %p, size %08lx\n", (void *)resource->base, resource->size);
}

static void exynos_displayport_set_resources(device_t dev)
{
	printk(BIOS_SPEW, "%s: dev %p\n", __function__, dev);
}

static void exynos_displayport_enable_resources(device_t dev)
{
	printk(BIOS_SPEW, "%s: dev %p\n", __function__, dev);
}

static struct device_operations exynos_displayport_operations  = {
	.read_resources   = exynos_displayport_read_resources,
	.set_resources    = exynos_displayport_set_resources,
	.enable_resources = exynos_displayport_enable_resources,
	.init		  = exynos_displayport_init,
	.scan_bus	  = exynos_displayport_noop,
};

static void exynos_displayport_enable(struct device *dev)
{
	printk(BIOS_SPEW, "%s: ", __function__);
	if (dev->link_list != NULL){
		printk(BIOS_SPEW, "set ops");
		dev->ops = &exynos_displayport_operations;
	}
	printk(BIOS_SPEW, "\n");
}

struct chip_operations drivers_i2c_exynos_displayport_ops = {
	CHIP_NAME("exynos displayport")
	.enable_dev = exynos_displayport_enable;
};
