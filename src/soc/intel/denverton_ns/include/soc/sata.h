/*
 * This file is part of the coreboot project.
 *
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
 */

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
