/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Joseph Smith <joe@settoplinux.org>
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
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cbfs.h>
#if CONFIG_PCI_OPTION_ROM_RUN_YABEL
#include <x86emu/x86emu.h>
#endif

static void vga_init(device_t dev)
{
	printk(BIOS_INFO, "Starting Graphics Initialization\n");
	struct cbfs_file *file = cbfs_get_file(CBFS_DEFAULT_MEDIA, "mbi.bin");
	void *mbi = NULL;
	unsigned int mbi_len = 0;

	if (file) {
		if (ntohl(file->type) != CBFS_TYPE_MBI) {
			printk(BIOS_INFO,  "CBFS:  MBI binary is of type %x instead of"
			       "type %x\n", file->type, CBFS_TYPE_MBI);
		} else {
			mbi = (void *) CBFS_SUBHEADER(file);
			mbi_len = ntohl(file->len);
		}
	} else {
		printk(BIOS_INFO,  "Could not find MBI.\n");
	}

	if (mbi && mbi_len) {
		/* The GDT or coreboot table is going to live here. But
		 * a long time after we relocated the GNVS, so this is
		 * not troublesome.
		 */
		*(u32 *)0x500 = (u32)mbi;
		*(u32 *)0x504 = (u32)mbi_len;
		outb(0xeb, 0xb2);
	}

	pci_dev_init(dev);
	printk(BIOS_INFO, "Graphics Initialization Complete\n");

	/* Enable TV-Out */
#if CONFIG_PCI_OPTION_ROM_RUN_YABEL
#define PIPE_A_CRT	(1 << 0)
#define PIPE_A_LFP	(1 << 1)
#define PIPE_A_TV	(1 << 3)
#define PIPE_B_CRT	(1 << 8)
#define PIPE_B_TV	(1 << 10)
	printk(BIOS_DEBUG, "Enabling TV-Out\n");
	void runInt10(void);
	X86_AX = 0x5f64;
	X86_BX = 0x0001; // Set Display Device, force execution
	X86_CX = PIPE_A_CRT | PIPE_A_TV;
	// M.x86.R_CX = PIPE_B_TV;
	runInt10();
	switch (X86_AX) {
	case 0x005f:
		printk(BIOS_DEBUG, "... failed.\n");
		break;
	case 0x015f:
		printk(BIOS_DEBUG, "... ok.\n");
		break;
	default:
		printk(BIOS_DEBUG, "... not supported.\n");
		break;
	}
#endif
}

static const struct device_operations vga_operations = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = vga_init,
	.scan_bus         = 0,
	.enable           = 0,
	.ops_pci          = 0,
};

static const struct pci_driver vga_driver __pci_driver = {
	.ops    = &vga_operations,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x3577,
};
