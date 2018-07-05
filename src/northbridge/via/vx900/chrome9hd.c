/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/vga_io.h>
#include <stdlib.h>

#include "vx900.h"

/**
 * @file chrome9hd.c
 *
 * \brief Initialization for Chrome9HD integrated graphics adapters
 *
 * This takes care of the initialization we need to do before calling the VGA
 * BIOS. The device is not documented in the VX900 datasheet.
 *
 * The device is documented in:
 *	Open Graphics Programming Manual
 *	Chrome9GraphicsHD Processor
 *	VX900 Series System Processor
 *	Part I: Graphics Core / 2D
 *
 * This document was released by VIA to the Xorg project, and is available at:
 * <http://www.x.org/docs/via/OGPM_Chrome9%20HD%20DX9%20_R100_PartI_Core_2D.pdf>
 *
 * STATUS:
 * We do the minimal initialization described in VIA documents. Running the VGA
 * option ROM does not get us a usable display. We configure the framebuffer and
 * the IGP is able to use it. GRUB2 and linux are capable of getting a usable
 * text console, which uses the monitor's native resolution (even 1920x1080).
 * The graphical console (linux) does not work properly.
 * @todo
 * 1. Figure out what sequence we need to do to get the VGA BIOS running
 *    properly. Use the code provided by VIA and compare their sequence to ours,
 *    fill in any missing steps, etc.
 * 2. Make BAR2 and the framebuffer use the same memory space. This is a feature
 *    called "Direct framebuffer access" which allows us to save memory space by
 *    setting BAR2 of the VGA to the location in memory of the framebuffer. This
 *    reduces the amount of PCI MMIO space we need below 4G, and is especially
 *    useful considering we only have 8GB (33 bits) of memory-mapped space.
 */

/**
 * vx900_int15
 *
 * \brief INT15 helpers for Chrome9HD IGP
 *
 * The following are helpers for INT15 handlers for the VGA BIOS. The full set
 * of INT15 callbacks is described in
 *
 *	VIA/S3Graphics
 *	Video BIOS External Interface Specification for Chrome9 Series IGP
 *	VX900 Series
 *
 * This document is only available under NDA, however, the callbacks are very
 * similar to other VIA/Intel IGP callbacks.
 *
 * Callback 0x5f18 is the most important one. It informs the VGA BIOS of the
 * RAM speed and framebuffer size. The other callbacks seem to be optional.
 * @{
 */

/**
 * \brief Get X86_BL value for VGA INT15 function 5f18
 *
 * Int15 5f18 lets the VGA BIOS know the framebuffer size and the memory speed.
 * This handler is very important. If it is not implemented, the VGA BIOS will
 * not work correctly.
 *
 * To use, just call this from the 15f18 handler, and place the return value in
 * X86_BL
 *
 * @code{.c}
 *	case 0x5f18:
 *		X86_BX = vx900_int15_get_5f18_bl();
 *		res = 0;
 *		break;
 * @endcode
 *
 */
u8 vx900_int15_get_5f18_bl(void)
{
	u8 reg8, ret;
	struct device *dev;
	/*
	 * BL Bit[7:4]
	 * Memory Data Rate (not to be confused with fCLK)
	 * 0000: 66MHz
	 * 0001: 100MHz
	 * 0010: 133MHz
	 * 0011: 200MHz ( DDR200 )
	 * 0100: 266MHz ( DDR266 )
	 * 0101: 333MHz ( DDR333 )
	 * 0110: 400MHz ( DDR400 )
	 * 0111: 533MHz ( DDR I/II 533)
	 * 1000: 667MHz ( DDR I/II 667)
	 * 1001: 800MHz  ( DDR3 800)
	 * 1010: 1066MHz ( DDR3 1066)
	 * 1011: 1333MHz ( DDR3 1333)
	 * Bit[3:0]
	 * N:  Frame Buffer Size 2^N  MB
	 */
	dev = dev_find_slot(0, PCI_DEVFN(0, 3));
	reg8 = pci_read_config8(dev, 0xa1);
	ret = (u32) ((reg8 & 0x70) >> 4) + 2;
	reg8 = pci_read_config8(dev, 0x90);
	reg8 = ((reg8 & 0x07) + 3) << 4;
	ret |= (u32) reg8;

	return ret;
}

/** @} */

static void chrome9hd_set_sid_vid(u16 vendor, u16 device)
{
	vga_sr_write(0x36, vendor >> 8);	/* SVID high byte */
	vga_sr_write(0x35, vendor & 0xff);	/* SVID low  byte */
	vga_sr_write(0x38, device >> 8);	/*  SID high byte */
	vga_sr_write(0x37, device & 0xff);	/*  SID low  byte */
}

static void chrome9hd_handle_uma(struct device *dev)
{
	u8 fb_pow = vx900_get_chrome9hd_fb_pow();

	if (fb_pow == 0)
		return;

	/* Step 7 - Let GFX know the framebuffer size (through PCI and IOCTL)
	 * The size we set here affects the behavior of BAR2, and the amount of
	 * MMIO space it requests. The default is 512MB, so if we don't set this
	 * before reading the resources, we could waste space below 4G */
	pci_write_config8(dev, 0xb2, ((0xff << (fb_pow - 2)) & ~(1 << 7)));
	vga_sr_write(0x68, (0xff << (fb_pow - 1)));
	/* And also that the framebuffer is in the system, RAM */
	pci_mod_config8(dev, 0xb0, 0, 1 << 0);
}

/**
 * \brief Initialization sequence before running the VGA BIOS
 *
 * This is the initialization sequence described in:
 *
 *	BIOS Porting Guide
 *	VX900 Series
 *	All-in-One System Processor
 *
 * This document is only available under NDA.
 */
static void chrome9hd_biosguide_init_seq(struct device *dev)
{
	struct device *mcu = dev_find_device(PCI_VENDOR_ID_VIA,
					    PCI_DEVICE_ID_VIA_VX900_MEMCTRL, 0);
	struct device *host = dev_find_device(PCI_VENDOR_ID_VIA,
					    PCI_DEVICE_ID_VIA_VX900_HOST_BR, 0);

	/* Step 1 - Enable VGA controller */
	/* FIXME: This is the VGA hole @ 640k-768k, and the vga port io
	 * We need the port IO, but can we disable the memory hole? */
	pci_mod_config8(mcu, 0xa4, 0, (1 << 7));	/* VGA memory hole */

	/* Step 2 - Forward MDA cycles to GFX */
	pci_mod_config8(host, 0x4e, 0, (1 << 1));

	/* Step 3 - Enable GFX I/O space */
	pci_mod_config8(dev, PCI_COMMAND, 0, PCI_COMMAND_IO);

	/* Step 4 - Enable video subsystem */
	vga_enable_mask((1 << 0), (1 << 0));

	/* FIXME: VGA IO Address Select. 3B5 or 3D5? */
	vga_misc_mask((1 << 0), (1 << 0));

	/* Step 5 - Unlock accessing of IO space */
	vga_sr_write(0x10, 0x01);

	chrome9hd_handle_uma(dev);

	uint64_t gfx_base = get_uma_memory_base();
	if (gfx_base == 0)
		die("uma_memory_base not set. Abandon ship!\n");

	/* Step 8 - Enable memory base register on the GFX */
	vga_sr_write(0x6d, (gfx_base >> 21) & 0xff);	/* base 28:21 */
	vga_sr_write(0x6e, (gfx_base >> 29) & 0xff);	/* base 36:29 */
	vga_sr_write(0x6f, 0x00);	/* base 43:37 */

	/* Step 9 - Set SID/VID */
	chrome9hd_set_sid_vid(0x1106, 0x7122);

}

static void chrome9hd_init(struct device *dev)
{
	printk(BIOS_DEBUG, "======================================================\n");
	printk(BIOS_DEBUG, "== Chrome9 HD INIT\n");
	printk(BIOS_DEBUG, "======================================================\n");

	chrome9hd_biosguide_init_seq(dev);

	/* Prime PLL FIXME: bad comment */
	vga_sr_mask(0x3c, 1 << 2, 1 << 2);

	/* FIXME: recheck; VGA IO Address Select. 3B5 or 3D5? */
	vga_misc_mask(1 << 0, 1 << 0);

	/* FIXME: recheck; Enable Base VGA 16 Bits Decode */

	u32 fb_address = pci_read_config32(dev, PCI_BASE_ADDRESS_2);
	fb_address &= ~0x0F;
	if (!fb_address) {
		printk(BIOS_WARNING, "Chrome9HD: No FB BAR assigned!\n");
		return;
	}

	printk(BIOS_INFO, "Chrome: Using %dMB Framebuffer at 0x%08X.\n",
	       256, fb_address);

	printk(BIOS_DEBUG, "Initializing VGA...\n");

	pci_dev_init(dev);

	printk(BIOS_DEBUG, "Enable VGA console\n");

	dump_pci_device(dev);
}

static void chrome9hd_enable(struct device *dev)
{
	struct device *mcu = dev_find_device(PCI_VENDOR_ID_VIA,
					    PCI_DEVICE_ID_VIA_VX900_MEMCTRL, 0);
	/* FIXME: here? -=- ACLK 250MHz */
	pci_mod_config8(mcu, 0xbb, 0, 0x01);
}

static void chrome9hd_disable(struct device *dev)
{
	struct device *mcu = dev_find_device(PCI_VENDOR_ID_VIA,
					    PCI_DEVICE_ID_VIA_VX900_MEMCTRL, 0);
	/* Disable GFX - This step effectively renders the GFX inert
	 * It won't even show up as a PCI device during enumeration */
	pci_mod_config8(mcu, 0xa1, 1 << 7, 0);
}

static struct device_operations chrome9hd_operations = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = chrome9hd_init,
	.disable = chrome9hd_disable,
	.enable = chrome9hd_enable,
	.ops_pci = 0,
};

static const struct pci_driver chrome9hd_driver __pci_driver = {
	.ops = &chrome9hd_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VX900_VGA,
};
