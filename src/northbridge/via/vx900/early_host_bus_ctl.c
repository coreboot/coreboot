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

static void vx900_cpu_bus_preram_setup(void)
{
	/* Faster CPU to DRAM Cycle */
	pci_mod_config8(HOST_BUS, 0x50, 0x0f, 0x08);
	/* CPU Interface Control – Basic Options */
	pci_mod_config8(HOST_BUS, 0x51, 0, 0x6c);
	/*CPU Interface Control – Advanced Options */
	pci_write_config8(HOST_BUS, 0x52, 0xc7);
	/* Enable 8QW burst and 4QW request merging [4] and [2]
	 * and special mode for read cycles bit[3] */
	pci_mod_config8(HOST_BUS, 0x54, 0, (1 << 4) | (1 << 2) | (1 << 3));
	/* High priority upstream requests on V4 bus */
	pci_write_config8(HOST_BUS, 0x56, 0x03);
	/* CPU to DRAM extra 1T access control */
	pci_mod_config8(HOST_BUS, 0x59, 0x00, (1 << 2));
	/* Queue reordering */
	pci_mod_config8(HOST_BUS, 0x5f, 0x00, (1 << 6));
	/* Only Write cycle of CPU->GFXCTL will flush the CPU->Memory FIFO */
	pci_mod_config8(HOST_BUS, 0x98, 0x00, 0x60);
	/* 1T delay for data on CPU bus */
	pci_write_config8(HOST_BUS, 0x9e, 0x0e);
	/* Arbitrate ownership of DRAM controller a few cycles earlier */
	pci_mod_config8(HOST_BUS, 0x9f, 0x00, (1 << 7));
	/* Write retire policy */
	pci_write_config8(HOST_BUS, 0x5d, 0xa2);
	/* Occupancy timer */
	pci_write_config8(HOST_BUS, 0x53, 0x44);
	/* Medium Threshold for Write Retire Policy - 6 requests */
	pci_mod_config8(HOST_BUS, 0x56, 0x00, 0x60);
	/* Bandwidth timer */
	pci_write_config8(HOST_BUS, 0x5e, 0x44);
}

/**
 * \brief Configure the CPU to northbridge bus (formerly, FSB)
 *
 * Configure the CPU <-> host interface. This interface is complex and needs to
 * be set up to operate properly. Configured parameters include bandwidth
 * arbitration. This function does not, however, change the physical interface
 * parameters, such as drive strength and signal timing. Instead, it assumes
 * that those parameters were already configured correctly from the ROMSTRAP.
 */
void vx900_cpu_bus_interface_setup(void)
{
	vx900_cpu_bus_preram_setup();

	dump_pci_device(HOST_BUS);
}
