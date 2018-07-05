/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Kyösti Mälkki <kyosti.malkki@gmail.com>
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

#ifndef DXPLPLUSU_BUS_H_INCLUDED
#define DXPLPLUSU_BUS_H_INCLUDED

/* These were determined by seeing how coreboot enumerates the various
 * PCI (and PCI-like) buses on the board.
 */

#define PCI_BUS_ROOT		0
#define PCI_BUS_AGP		1	/* AGP */
#define PCI_BUS_E7501_HI_B	2	/* P64H2#1 */
#define PCI_BUS_P64H2_B		3	/* P64H2#1 bus B */
#define PCI_BUS_P64H2_A		4	/* P64H2#1 bus A */
#define PCI_BUS_ICH4		5	/* ICH4 */

/* IOAPIC addresses determined by coreboot enumeration. */
/* Someday add functions to get APIC IDs and versions from the chips themselves. */

#define IOAPIC_ICH4		2
#define IOAPIC_P64H2_BUS_B	3	/* IOAPIC 3 at 02:1c.0  MBAR = fe300000 DataAddr = fe300010 */
#define IOAPIC_P64H2_BUS_A	4	/* IOAPIC 4 at 02:1e.0  MBAR = fe301000 DataAddr = fe301010 */

#define INTEL_IOAPIC_NUM_INTERRUPTS	24	/* Both ICH-4 and P64-H2 */

#endif
