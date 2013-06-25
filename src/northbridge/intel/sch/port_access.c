/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009-2010 iWave Systems
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
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

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <stdint.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include "sch.h"

/*
 * Restricted Access Regions:
 *
 * MCR - Message Control Register
 * 31        24              16                 8              4              0
 * ----------------------------------------------------------------------------
 * |          |               |    Target       |   Write      |              |
 * | Opcode   |  Port         |    register     |   byte       |   Reserved   |
 * |          |               |    Address      |   Enables    |              |
 * ----------------------------------------------------------------------------
 *
 * MDR - Message Data Register
 * 31                                                                         0
 * ----------------------------------------------------------------------------
 * |                                                                          |
 * |                            Data                                          |
 * |                                                                          |
 * ----------------------------------------------------------------------------
 */

#define MSG_OPCODE_READ  0xD0000000
#define MSG_OPCODE_WRITE 0xE0000000

#define MCR 0xD0
#define MDR 0xD4

int sch_port_access_read(int port, int reg, int bytes)
{
	pci_write_config32(PCI_DEV(0, 0, 0), MCR,
			   (MSG_OPCODE_READ | (port << 16) | (reg << 8)));
	return pci_read_config32(PCI_DEV(0, 0, 0), MDR);
}

void sch_port_access_write(int port, int reg, int bytes, long data)
{
	pci_write_config32(PCI_DEV(0, 0, 0), MDR, data);
	pci_write_config32(PCI_DEV(0, 0, 0), MCR,
			   (MSG_OPCODE_WRITE | (port << 16) | (reg << 8)));
	pci_read_config32(PCI_DEV(0, 0, 0), MDR);
}

void sch_port_access_write_ram_cmd(int cmd, int port, int reg, int data)
{
	pci_write_config32(PCI_DEV(0, 0, 0), MDR, data);
	pci_write_config32(PCI_DEV(0, 0, 0), MCR,
			   ((cmd << 24) | (port << 16) | (reg << 8)));
	pci_read_config32(PCI_DEV(0, 0, 0), MDR);
}
