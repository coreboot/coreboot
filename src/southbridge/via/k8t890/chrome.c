/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 Luc Verhaegen <libv@skynet.be>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h> /* for memset */
#include "k8x8xx.h"

#if CONFIG_VGA
#include <pc80/vga_io.h>
#include <pc80/vga.h>
#include <arch/io.h>

/*
 *
 */
static void
chrome_vga_init(struct device *dev)
{
	vga_sr_write(0x10, 0x01); /* unlock extended regs */

	vga_sr_mask(0x1A, 0x02, 0x02); /* enable mmio */

	vga_sr_mask(0x1A, 0x40, 0x40); /* Software Reset */

	vga_cr_mask(0x6A, 0x00, 0xC8); /* Disable CRTC2 & Simultaneous */

	/* Make sure that non of the primary VGA overflow registers are set */
	vga_cr_write(0x33, 0x00);
	vga_cr_write(0x35, 0x00);
	vga_cr_mask(0x11, 0x00, 0x30);

	vga_sr_mask(0x16, 0x00, 0x40); /* Wire CRT to CRTC1 */
	vga_cr_mask(0x36, 0x00, 0x30); /* Power on CRT */

	/* Disable Extended Display Mode */
	vga_sr_mask(0x15, 0x00, 0x02);

	/* Disable Wrap-around */
	vga_sr_mask(0x15, 0x00, 0x20);

	/* Disable Extended Mode memory access */
	vga_sr_mask(0x1A, 0x00, 0x08);

	/* Make sure that we only touch CRTC1s DAC */
	vga_sr_mask(0x1A, 0x00, 0x01);

	/* Set up power to the clocks/crtcs */
	vga_sr_mask(0x19, 0x7F, 0x7F); /* enable clock gating for all. */
	vga_sr_mask(0x1B, 0xC0, 0xC0); /* secondary clock according to pm */
	vga_sr_mask(0x1B, 0x20, 0x30); /* primary clock is always on */

	/* set everything according to PM/Engine idle state except pci dma */
	vga_sr_write(0x2D, 0xFF); /* Power management control 1 */
	vga_sr_write(0x2E, 0xFB); /* Power management control 2 */
	vga_sr_write(0x3F, 0xFF); /* Power management control 3 */

	/* now set up the engine clock. */
	vga_sr_write(0x47, 0xB8);
	vga_sr_write(0x48, 0x08);
	vga_sr_write(0x49, 0x03);

	/* trigger engine clock setting */
	vga_sr_mask(0x40, 0x01, 0x01);
	vga_sr_mask(0x40, 0, 0x01);

	vga_cr_mask(0x30, 0x04, 0x04); /* Enable PowerNow in primary path */
	vga_cr_mask(0x36, 0x01, 0x01); /* Enable PCI Power Management */

	/* Power now indicators... */
	vga_cr_write(0x41, 0xB9);
	vga_cr_write(0x42, 0xB4);
	/* could these be the CRTC2 power now indicators? */
	vga_cr_write(0x9D, 0x80); /* Power Now Ending position enable */
	vga_cr_write(0x9E, 0xB4); /* Power Now Control 3 */

	/* primary fifo setting */
	vga_sr_mask(0x16, 0x28, 0xBF); /* pthreshold: 160 */
	vga_sr_write(0x17, 0x60); /* max depth: 194 */
	vga_sr_mask(0x18, 0x0E, 0xBF); /* high priority threshold: 56 */
	vga_sr_write(0x1C, 0x54); /* Fetch count */

	vga_sr_write(0x20, 0x40); /* display queue typical arbiter control 0 */
	vga_sr_write(0x21, 0x40); /* display queue typical arbiter control 1 */
	vga_sr_mask(0x22, 0x14, 0x1F); /* display queue expire number */

	/* Typical Arbiter Control */
	vga_sr_mask(0x41, 0x40, 0xF0); /* Request threshold */
	vga_sr_mask(0x42, 0x20, 0x20); /* Support Fetch Cycle with Length 2 */

	vga_sr_write(0x50, 0x1F); /* AGP Control Register */
	vga_sr_write(0x51, 0xF5); /* AGP FIFO Control 1 */

	vga_cr_mask(0x33, 0x08, 0x08); /* Enable Prefetch Mode */
}

#endif /* CONFIG_VGA */

/*
 *
 */
static void
chrome_init(struct device *dev)
{
	uint32_t fb_size, fb_address;

	fb_size = k8m890_host_fb_size_get();
	if (!fb_size) {
		printk(BIOS_WARNING, "Chrome: Device has not been initialised in the"
			       " ramcontroller!\n");
		return;
	}

	fb_address = pci_read_config32(dev, 0x10);
	fb_address &= ~0x0F;
	if (!fb_address) {
		printk(BIOS_WARNING, "Chrome: No FB BAR assigned!\n");
		return;
	}

	printk(BIOS_INFO, "Chrome: Using %dMB Framebuffer at 0x%08X.\n",
		    fb_size, fb_address);

	//k8m890_host_fb_direct_set(fb_address);

#if CONFIG_VGA
	/* Now set up the VGA console */
	vga_io_init(); /* Enable full IO access */

	chrome_vga_init(dev);

	vga_textmode_init();

	printk(BIOS_INFO, "Chrome VGA Textmode initialized.\n");

	/* if we don't have console, at least print something... */
	vga_line_write(0, "Chrome VGA Textmode initialized.");
#endif /* CONFIG_VGA */
}

static struct device_operations
chrome_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = chrome_init,
	.scan_bus         = 0,
	.enable           = 0,
};

static const struct pci_driver unichrome_driver_800 __pci_driver = {
	.ops    = &chrome_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_K8M800_CHROME,
};

static const struct pci_driver unichrome_driver_890 __pci_driver = {
	.ops    = &chrome_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_K8M890_CHROME,
};
