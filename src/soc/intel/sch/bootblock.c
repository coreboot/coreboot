/*
 * This file is part of the coreboot project.
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
 */

#include <arch/io.h>

/* Copy the bare minimum from port_access.c to enable MMCONF. */

#define MSG_OPCODE_READ  0xD0000000
#define MSG_OPCODE_WRITE 0xE0000000

#define MCR 0xD0
#define MDR 0xD4

static void sch_port_access_write(int port, int reg, int bytes, long data)
{
	pci_io_write_config32(PCI_DEV(0, 0, 0), MDR, data);
	pci_io_write_config32(PCI_DEV(0, 0, 0), MCR,
			   (MSG_OPCODE_WRITE | (port << 16) | (reg << 8)));
	pci_io_read_config32(PCI_DEV(0, 0, 0), MDR);
}

static void bootblock_northbridge_init(void)
{
	/* Enable PCI MMCONF decoding BAR. */
	sch_port_access_write(0, 0, 4, DEFAULT_PCIEXBAR | 1); /* pre-b1 */
	sch_port_access_write(2, 9, 4, DEFAULT_PCIEXBAR | 1); /* b1+ */
}
