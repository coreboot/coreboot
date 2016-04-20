/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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

#include <soc/irq.h>
#include <soc/pci_devs.h>

#define PCI_DEV_PIRQ_ROUTES \
	PCI_DEV_PIRQ_ROUTE(XHCI_DEV,  A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(ME_DEV,    A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(GBE_DEV,   A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(EHCI2_DEV, A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(HDA_DEV,   A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(PCIE_DEV,  A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(EHCI1_DEV, A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(SATA_DEV,  A, B, C, D)

/*
 * Route each PIRQ[A-H] to a PIC IRQ[0-15]
 * Reserved: 0, 1, 2, 8, 13
 * ACPI/SCI: 9
 */
#define PIRQ_PIC_ROUTES \
	PIRQ_PIC(A,  5), \
	PIRQ_PIC(B,  6), \
	PIRQ_PIC(C,  7), \
	PIRQ_PIC(D, 10), \
	PIRQ_PIC(E, 11), \
	PIRQ_PIC(F, 12), \
	PIRQ_PIC(G, 14), \
	PIRQ_PIC(H, 15)

#endif /* IRQROUTE_H */
