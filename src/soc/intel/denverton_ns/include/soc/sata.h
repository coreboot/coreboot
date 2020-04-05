/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _DENVERTON_NS_SATA_H
#define _DENVERTON_NS_SATA_H

/* PCI Configuration Space (D19:F0): SATA #0 */
/* PCI Configuration Space (D20:F0): SATA #1 */
#define PCH_SATA0_DEV PCI_DEV(0, SATA_DEV, SATA_FUNC)
#define PCH_SATA1_DEV PCI_DEV(0, SATA2_DEV, SATA2_FUNC)

#define SATAGC 0x9c
#define SATAGC_AHCI (0 << 16)
#define SATAGC_RAID (1 << 16)

#endif //_DENVERTON_NS_SATA_H
