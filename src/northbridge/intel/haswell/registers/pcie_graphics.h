/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef HASWELL_REGISTERS_PCIE_GRAPHICS_H
#define HASWELL_REGISTERS_PCIE_GRAPHICS_H

/* Device 0:1.0 PCI configuration space (PCIe Graphics) */
#define PEG_CAP		0xa2
#define PEG_DCAP	0xa4

#define PEG_LCAP	0xac

#define PEG_DSTS	0xaa

#define PEG_SLOTCAP	0xb4

#define PEG_DCAP2	0xc4	/* 32bit */

#define PEG_LCTL2	0xd0

#define PEG_VC0RCTL	0x114

#define PEG_ESD		0x144	/* 32bit */
#define PEG_LE1D	0x150	/* 32bit */
#define PEG_LE1A	0x158	/* 64bit */

#define PEG_UESTS	0x1c4
#define PEG_UESEV	0x1cc
#define PEG_CESTS	0x1d0

#define PEG_L0SLAT	0x22c

#define PEG_AFE_PM_TMR	0xc28

#endif /* HASWELL_REGISTERS_PCIE_GRAPHICS_H */
