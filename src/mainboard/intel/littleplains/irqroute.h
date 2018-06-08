/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2014 Sage Electronics Engineering, LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef IRQROUTE_H
#define IRQROUTE_H

#include <southbridge/intel/fsp_rangeley/irq.h>
#include <southbridge/intel/fsp_rangeley/pci_devs.h>

/*
 * IR01h PCIe		INT(ABCD)	- PIRQ ABCD
 * IR02h PCIe		INT(ABCD)	- PIRQ ABCD
 * IR03h PCIe		INT(ABCD)	- PIRQ ABCD
 * IR04h PCIe		INT(ABCD)	- PIRQ ABCD
 * IR0Bh IQIA		INT(ABCD)	- PIRQ EFGH
 * IR0Eh RAS		INT(A)		- PIRQ A
 * IR13h SMBUS1		INT(A)		- PIRQ B
 * IR15h GBE		INT(A)		- PIRQ CDEF
 * IR1Dh EHCI		INT(A)		- PIRQ G
 * IR13h SATA2.0	INT(A)		- PIRQ H
 * IR13h SATA3.0	INT(A)		- PIRQ A
 * IR1Fh LPC		INT(ABCD)	- PIRQ HGBC
 */
#define PCI_DEV_PIRQ_ROUTES \
	PCI_DEV_PIRQ_ROUTE(PCIE_PORT1_DEV,	A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(PCIE_PORT2_DEV,	D, C, B, A), \
	PCI_DEV_PIRQ_ROUTE(PCIE_PORT3_DEV,	E, F, G, H), \
	PCI_DEV_PIRQ_ROUTE(PCIE_PORT4_DEV,	H, G, F, E), \
	PCI_DEV_PIRQ_ROUTE(IQAT_DEV,		E, F, G, H), \
	PCI_DEV_PIRQ_ROUTE(HOST_BRIDGE_DEV,	H, A, A, A), \
	PCI_DEV_PIRQ_ROUTE(RCEC_DEV,		A, A, A, B), \
	PCI_DEV_PIRQ_ROUTE(SMBUS1_DEV,		B, A, A, A), \
	PCI_DEV_PIRQ_ROUTE(GBE_DEV,			C, D, E, F), \
	PCI_DEV_PIRQ_ROUTE(USB2_DEV,		G, A, A, A), \
	PCI_DEV_PIRQ_ROUTE(SATA2_DEV,		H, A, A, A), \
	PCI_DEV_PIRQ_ROUTE(SATA3_DEV,		A, A, A, B), \
	PCI_DEV_PIRQ_ROUTE(PCU_DEV,			H, G, B, C)

/*
 * Route each PIRQ[A-H] to a PIC IRQ[0-15]
 * Reserved: 0, 1, 2, 8, 13
 * PS2 keyboard: 12
 * ACPI/SCI: 9
 * Floppy: 6
 */
#define PIRQ_PIC_ROUTES \
	PIRQ_PIC(A, 10),  \
	PIRQ_PIC(B, 11),  \
	PIRQ_PIC(C, 10),  \
	PIRQ_PIC(D, 11),  \
	PIRQ_PIC(E, 14), \
	PIRQ_PIC(F, 15), \
	PIRQ_PIC(G, 14), \
	PIRQ_PIC(H, 15)

#endif /* IRQROUTE_H */
