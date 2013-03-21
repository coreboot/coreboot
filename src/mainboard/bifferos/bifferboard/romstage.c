/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Rudolf Marek <r.marek@assembler.cz>
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

#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/x86/cache.h>

static void main(void)
{
	uint32_t tmp;
	post_code(0x05);

	/* Set timer1 to pulse generator 15us for memory refresh */
	outb(0x54, 0x43);
	outb(0x12, 0x41);

	/* CPU setup, romcc pukes on invd() */
	asm volatile ("invd");
	enable_cache();

	/* Set serial base */
	pci_write_config32(PCI_DEV(0,7,0), 0x54, 0x3f8);
	/* serial IRQ disable, LPC disable, COM2 goes to LPC, internal UART for COM1 */
	pci_write_config32(PCI_DEV(0,7,0), 0x50, 0x84101012);

	console_init();

	/* memory init */
	pci_write_config32(PCI_DEV(0,0,0), 0x68, 0x6c99f);
	pci_write_config32(PCI_DEV(0,0,0), 0x6c, 0x800451);
	pci_write_config32(PCI_DEV(0,0,0), 0x70, 0x4000003);

	/* memory phase/buffer strength for read and writes */
	tmp = pci_read_config32(PCI_DEV(0,0,0), 0x64);
	tmp &= 0x0FF00FFFF;
	tmp |= 0x790000;
	pci_write_config32(PCI_DEV(0,0,0), 0x64, tmp);
	/* Route Cseg, Dseg, Eseg and Fseg to RAM */
	pci_write_config32(PCI_DEV(0,0,0), 0x84, 0x3ffffff0);
}

