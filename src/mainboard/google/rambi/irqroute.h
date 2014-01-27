/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <soc/intel/baytrail/baytrail/irq.h>
#include <soc/intel/baytrail/baytrail/pci_devs.h>

#define PCI_DEV_PIRQ_ROUTES \
	PCI_DEV_PIRQ_ROUTE(GFX_DEV,  A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(SDIO_DEV, A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(SD_DEV,   C, D, E, F), \
	PCI_DEV_PIRQ_ROUTE(SATA_DEV, A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(XHCI_DEV, A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(LPE_DEV,  A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(MMC_DEV,  D, E, F, G), \
	PCI_DEV_PIRQ_ROUTE(SIO1_DEV, A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(TXE_DEV,  A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(HDA_DEV,  A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(PCIE_DEV, A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(EHCI_DEV, A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(SIO2_DEV, B, C, D, E), \
	PCI_DEV_PIRQ_ROUTE(PCU_DEV,  A, B, C, D)

#define PIRQ_PIC_ROUTES \
	PIRQ_PIC(A, DISABLE), \
	PIRQ_PIC(B, DISABLE), \
	PIRQ_PIC(C, DISABLE), \
	PIRQ_PIC(D, DISABLE), \
	PIRQ_PIC(E, DISABLE), \
	PIRQ_PIC(F, DISABLE), \
	PIRQ_PIC(G, DISABLE), \
	PIRQ_PIC(H, DISABLE)

/* CORE bank DIRQs - up to 16 supported */
#define TPAD_IRQ_OFFSET		0
#define TOUCH_IRQ_OFFSET	1
/* Corresponding SCORE GPIO pins */
#define TPAD_IRQ_GPIO		55
#define TOUCH_IRQ_GPIO		72
