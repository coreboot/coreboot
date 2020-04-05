/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <soc/irq.h>
#include <soc/pci_devs.h>
#include <soc/pmc.h>

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
#define I8042_IRQ_OFFSET	2
#define ALS_IRQ_OFFSET		3
/* Corresponding SCORE GPIO pins */
#define TPAD_IRQ_GPIO		55
#define TOUCH_IRQ_GPIO		72
#define I8042_IRQ_GPIO		101
#define ALS_IRQ_GPIO		70

/* SUS bank DIRQs - up to 16 supported */
#define CODEC_IRQ_OFFSET	0
/* Corresponding SUS GPIO pins */
#define CODEC_IRQ_GPIO		9
