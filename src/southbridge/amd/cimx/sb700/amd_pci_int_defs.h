/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
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

#ifndef AMD_PCI_INT_DEFS_H
#define AMD_PCI_INT_DEFS_H

/*
 * PIRQ and device routing - these define the index
 * into the FCH PCI_INTR 0xC00/0xC01 interrupt
 * routing table
 */
#define FCH_INT_TABLE_SIZE 0xD
#define PIRQ_NC		0x1F	/* Not Used */
#define PIRQ_A		0x00	/* INT A */
#define PIRQ_B		0x01	/* INT B */
#define PIRQ_C		0x02	/* INT C */
#define PIRQ_D		0x03	/* INT D */
#define PIRQ_ACPI	0x04	/* ACPI */
#define PIRQ_SMBUS	0x05	/* SMBUS */
/* Index 6, 7, 8 are all reserved */
#define PIRQ_E		0x09	/* INT E */
#define PIRQ_F		0x0A	/* INT F */
#define PIRQ_G		0x0B	/* INT G */
#define PIRQ_H		0x0C	/* INT H */

#endif /* AMD_PCI_INT_DEFS_H */
