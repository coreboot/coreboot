/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009-2010 iWave Systems
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
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
 * Foundation, Inc.
 */

#ifndef __PRE_RAM__
#define __PRE_RAM__ // Use simple device model for this file even in ramstage
#endif

#include <stdint.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include "northbridge.h"

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

#define MSG_OPCODE_READ  0x10 << 24
#define MSG_OPCODE_WRITE 0x11 << 24

#define MCR 0xD0
#define MDR 0xD4
#define MCRE 0xD8

u32 sideband_read(int port, int reg)
{
	pci_write_config32(PCI_DEV(0, 0, 0), MCR,
			   (MSG_OPCODE_READ | (port << 16) | (reg << 8)));
	return pci_read_config32(PCI_DEV(0, 0, 0), MDR);
}

void sideband_write(int port, int reg, long data)
{
	pci_write_config32(PCI_DEV(0, 0, 0), MDR, data);
	pci_write_config32(PCI_DEV(0, 0, 0), MCR,
			   (MSG_OPCODE_WRITE | (port << 16) | (reg << 8) | (0xF << 4)));
	pci_read_config32(PCI_DEV(0, 0, 0), MDR);
}
