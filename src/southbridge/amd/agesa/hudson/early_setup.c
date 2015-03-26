/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
 * Foundation, Inc.
 */

#ifndef _HUDSON_EARLY_SETUP_C_
#define _HUDSON_EARLY_SETUP_C_

#include <stdint.h>
#include <arch/io.h>
#include <arch/acpi.h>
#include <console/console.h>
#include <reset.h>
#include <arch/cpu.h>
#include <cbmem.h>
#include "hudson.h"

void hudson_pci_port80(void)
{
	u8 byte;
	device_t dev;

	/* P2P Bridge */
	dev = PCI_DEV(0, 0x14, 4);

	/* Chip Control: Enable subtractive decoding */
	byte = pci_read_config8(dev, 0x40);
	byte |= 1 << 5;
	pci_write_config8(dev, 0x40, byte);

	/* Misc Control: Enable subtractive decoding if 0x40 bit 5 is set */
	byte = pci_read_config8(dev, 0x4B);
	byte |= 1 << 7;
	pci_write_config8(dev, 0x4B, byte);

	/* The same IO Base and IO Limit here is meaningful because we set the
	 * bridge to be subtractive. During early setup stage, we have to make
	 * sure that data can go through port 0x80.
	 */
	/* IO Base: 0xf000 */
	byte = pci_read_config8(dev, 0x1C);
	byte |= 0xF << 4;
	pci_write_config8(dev, 0x1C, byte);

	/* IO Limit: 0xf000 */
	byte = pci_read_config8(dev, 0x1D);
	byte |= 0xF << 4;
	pci_write_config8(dev, 0x1D, byte);

	/* PCI Command: Enable IO response */
	byte = pci_read_config8(dev, 0x04);
	byte |= 1 << 0;
	pci_write_config8(dev, 0x04, byte);

	/* LPC controller */
	dev = PCI_DEV(0, 0x14, 3);

	byte = pci_read_config8(dev, 0x4A);
	byte &= ~(1 << 5); /* disable lpc port 80 */
	pci_write_config8(dev, 0x4A, byte);
}

void hudson_lpc_port80(void)
{
	u8 byte;
	device_t dev;

	/* Enable LPC controller */
	outb(0xEC, 0xCD6);
	byte = inb(0xCD7);
	byte |= 1;
	outb(0xEC, 0xCD6);
	outb(byte, 0xCD7);

	/* Enable port 80 LPC decode in pci function 3 configuration space. */
	dev = PCI_DEV(0, 0x14, 3);
	byte = pci_read_config8(dev, 0x4a);
	byte |= 1 << 5; /* enable port 80 */
	pci_write_config8(dev, 0x4a, byte);
}

int s3_save_nvram_early(u32 dword, int size, int  nvram_pos)
{
	int i;
	printk(BIOS_DEBUG, "Writing %x of size %d to nvram pos: %d\n", dword, size, nvram_pos);

	for (i = 0; i<size; i++) {
		outb(nvram_pos, BIOSRAM_INDEX);
		outb((dword >>(8 * i)) & 0xff , BIOSRAM_DATA);
		nvram_pos++;
	}

	return nvram_pos;
}

int s3_load_nvram_early(int size, u32 *old_dword, int nvram_pos)
{
	u32 data = *old_dword;
	int i;
	for (i = 0; i<size; i++) {
		outb(nvram_pos, BIOSRAM_INDEX);
		data &= ~(0xff << (i * 8));
		data |= inb(BIOSRAM_DATA) << (i *8);
		nvram_pos++;
	}
	*old_dword = data;
	printk(BIOS_DEBUG, "Loading %x of size %d to nvram pos:%d\n", *old_dword, size,
		nvram_pos-size);
	return nvram_pos;
}

#endif /* _HUDSON_EARLY_SETUP_C_ */
