/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include "chip.h"
#include "northbridge.h"

/* PCI Domain 1 Device 0 Function 0 */

#define SR_INDEX	0x3c4
#define SR_DATA		0x3c5
#define CRTM_INDEX	0x3b4
#define CRTM_DATA	0x3b5
#define CRTC_INDEX	0x3d4
#define CRTC_DATA	0x3d5

void setup_realmode_idt(void);
void do_vgabios(void);
void vga_enable_console(void);

void write_protect_vgabios(void)
{
	device_t dev;

	printk(BIOS_DEBUG, "write_protect_vgabios\n");

	dev = dev_find_device(PCI_VENDOR_ID_VIA, 0x3324, 0);
	if (dev)
		pci_write_config8(dev, 0x80, 0xff);

	dev = dev_find_device(PCI_VENDOR_ID_VIA, 0x7324, 0);
	if (dev)
		pci_write_config8(dev, 0x61, 0xff);
}

static void vga_init(device_t dev)
{
	u8 reg8;

	printk(BIOS_DEBUG, "Initializing VGA...\n");

	//*
	pci_write_config8(dev, 0x04, 0x07);
	pci_write_config8(dev, 0x3e, 0x02);
	pci_write_config8(dev, 0x0d, 0x40);
	pci_write_config32(dev, 0x10, 0xa0000008);
	pci_write_config32(dev, 0x14, 0xdd000000);
	pci_write_config8(dev, 0x3c, 0x0b);
	//*/

	printk(BIOS_DEBUG, "Executing VGA option rom in real mode\n");
	setup_realmode_idt();
	do_vgabios();
	printk(BIOS_DEBUG, "Enable VGA console\n");
	vga_enable_console();

	/* It's not clear if these need to be programmed before or after
	 * the VGA bios runs. Try both, clean up later */
	/* Set memory rate to 200MHz */
	outb(0x3d, CRTM_INDEX);
	reg8 = inb(CRTM_DATA);
	reg8 &= 0x0f;
	reg8 |= (0x3 << 4);
	outb(0x3d, CRTM_INDEX);
	outb(reg8, CRTM_DATA);

	/* Set framebuffer size to 32mb */
	reg8 = (32 / 4);
	outb(0x39, SR_INDEX);
	outb(reg8, SR_DATA);
}

static struct device_operations vga_operations = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = vga_init,
	.ops_pci = 0,
};

static const struct pci_driver vga_driver __pci_driver = {
	.ops = &vga_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = 0x3157,
};
