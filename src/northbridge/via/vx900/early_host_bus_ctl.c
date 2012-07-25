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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "early_vx900.h"

static void vx900_cpu_bus_immitate_vendor_bios(void)
{
	int i = 0;
	printk(BIOS_DEBUG, "Immiation step %3u\n", i++);
	/* Dynamic Defer Snoop Stall Count - 16 clocks */
	pci_mod_config8(HOST_BUS, 0x50, 0x0f, 0x80);
	printk(BIOS_DEBUG, "Immiation step %3u\n", i++);
	/* */
	pci_mod_config8(HOST_BUS, 0x51, 0, (1<<5) | (1<<3) | (1<<2) | (1<<6) );
	//pci_mod_config8(HOST_BUS, 0x51, 0, (1<<5));
	printk(BIOS_DEBUG, "Immiation step %3u\n", i++);
	pci_write_config8(HOST_BUS, 0x52, 0xc7);
	printk(BIOS_DEBUG, "Immiation step %3u\n", i++);

	pci_write_config8(HOST_BUS, 0x56, 0x03);
	printk(BIOS_DEBUG, "Immiation step %3u\n", i++);

	/* CPU to DRAM extra 1T access control */
	pci_mod_config8(HOST_BUS, 0x59, 0x00, 1<<2);
	printk(BIOS_DEBUG, "Immiation step %3u\n", i++);

	/* Queue reordering */
	pci_mod_config8(HOST_BUS, 0x5f, 0x00, 1<<6);
	printk(BIOS_DEBUG, "Immiation step %3u\n", i++);

	/* Only Write cycle of CPU->GFXCTL will flush the CPU->Memory FIFO */
	pci_mod_config8(HOST_BUS, 0x98, 0x00, 0x60);
	printk(BIOS_DEBUG, "Immiation step %3u\n", i++);

	/* 1T delay for data on CPU bus */
	pci_write_config8(HOST_BUS, 0x9e, 0x0e);
	printk(BIOS_DEBUG, "Immiation step %3u\n", i++);

	/* Arbitrate owership of DRAM controller a few cycles earlier */
	pci_mod_config8(HOST_BUS, 0x9f, 0x00, 1<<7);
	printk(BIOS_DEBUG, "Immiation step %3u\n", i++);

	//pci_mod_config8(HOST_BUS, 0x51, 0, 0x0c);

	/* Write retire policy */
	pci_write_config8(HOST_BUS, 0x5d, 0xa2);
	printk(BIOS_DEBUG, "Immiation step %3u\n", i++);

	//pci_mod_config8(HOST_BUS, 0x51, 0, 1<<6);

	/* Occupancy timer */
	pci_write_config8(HOST_BUS, 0x53, 0x44);
	printk(BIOS_DEBUG, "Immiation step %3u\n", i++);

	/* Medium Threshold for Write Retire Policy - 6 requests */
	pci_mod_config8(HOST_BUS, 0x56, 0x00, 0x60);
	printk(BIOS_DEBUG, "Immiation step %3u\n", i++);

	/* Bandwidth timer */
	pci_write_config8(HOST_BUS, 0x5e, 0x44);
	printk(BIOS_DEBUG, "Immiation step %3u\n", i++);

}

void vx900_cpu_bus_slowest_rdry(void)
{
	/* Disable fast CPU to DRAM cycle, otherwise we might hang on raminit */
	pci_mod_config8(HOST_BUS, 0x51, 1<<7, 0);
	/* Memory to CPU synchronous mode */
	pci_mod_config8(HOST_BUS, 0x51, 1<<1, 0);
}

void vx900_cpu_bus_interface_setup(void)
{
	/* Enable 8QW burst and 4QW request merging [4] and [2]
	 * and special mode for read cycles bit[3] */
	//pci_mod_config8(HOST_BUS, 0x54, 0, (1<<4) | (1<<2) | (1<<3) );

	/* This is good practice to do before raminit */
	//vx900_cpu_bus_slowest_rdry();

	vx900_cpu_bus_immitate_vendor_bios();

	dump_pci_device(HOST_BUS);
}