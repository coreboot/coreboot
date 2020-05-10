/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/irq.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

#define PCI_DEV_PIRQ_ROUTES \
	PCI_DEV_PIRQ_ROUTE(GFX_DEV,    A, A, A, A), \
	PCI_DEV_PIRQ_ROUTE(SATA_DEV,   D, A, A, A), \
	PCI_DEV_PIRQ_ROUTE(XHCI_DEV,   E, A, A, A), \
	PCI_DEV_PIRQ_ROUTE(SIO1_DEV,   B, A, D, C), \
	PCI_DEV_PIRQ_ROUTE(TXE_DEV,    F, A, A, A), \
	PCI_DEV_PIRQ_ROUTE(HDA_DEV,    G, A, A, A), \
	PCI_DEV_PIRQ_ROUTE(PCIE_DEV,   A, B, C, D), \
	PCI_DEV_PIRQ_ROUTE(SIO2_DEV,   D, B, C, A), \
	PCI_DEV_PIRQ_ROUTE(PCU_DEV,    A, C, A, A)

#define PIRQ_PIC_ROUTES \
	PIRQ_PIC(A, 11), \
	PIRQ_PIC(B, 5), \
	PIRQ_PIC(C, 5), \
	PIRQ_PIC(D, 11), \
	PIRQ_PIC(E, 11), \
	PIRQ_PIC(F, 5), \
	PIRQ_PIC(G, 11), \
	PIRQ_PIC(H, 11)
