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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef  _HUDSON_EARLY_SETUP_C_
#define  _HUDSON_EARLY_SETUP_C_

#include <stdint.h>
#include <arch/io.h>
#include <arch/acpi.h>
#include <console/console.h>
#include <reset.h>
#include <arch/cpu.h>
#include <cbmem.h>
#include "hudson.h"

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
	dev = PCI_DEV(0, 0x14, 3);//pci_locate_device(PCI_ID(0x1002, 0x439D), 0);
	byte = pci_read_config8(dev, 0x4a);
	byte |= 1 << 5;		/* enable port 80 */
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

#if CONFIG_HAVE_ACPI_RESUME
int acpi_get_sleep_type(void)
{
	u16 tmp = inw(PM1_CNT_BLK_ADDRESS);
	tmp = ((tmp & (7 << 10)) >> 10);
	/* printk(BIOS_DEBUG, "SLP_TYP type was %x\n", tmp); */
	return (int)tmp;
}
#endif

#if CONFIG_HAVE_ACPI_RESUME
int acpi_is_wakeup_early(void)
{
	return (acpi_get_sleep_type() == 3);
}
#endif

unsigned long get_top_of_ram(void)
{
	uint32_t xdata = 0;
	int xnvram_pos = 0xf8, xi;
	for (xi = 0; xi<4; xi++) {
		outb(xnvram_pos, BIOSRAM_INDEX);
		xdata &= ~(0xff << (xi * 8));
		xdata |= inb(BIOSRAM_DATA) << (xi *8);
		xnvram_pos++;
	}
	return (unsigned long) xdata;
}

#endif
