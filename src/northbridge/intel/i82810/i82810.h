/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Corey Osgood <corey.osgood@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#ifndef NORTHBRIDGE_INTEL_I82810_I82810_H
#define NORTHBRIDGE_INTEL_I82810_I82810_H

/*
 * Datasheet:
 *   - Name: Intel 810 Chipset:
 *           82810/82810-DC100 Graphics and Memory Controller Hub (GMCH)
 *   - URL: http://www.intel.com/design/chipsets/datashts/290656.htm
 *   - PDF: ftp://download.intel.com/design/chipsets/datashts/29065602.pdf
 *   - Order Number: 290656-002
 */

/*
 * PCI Configuration Registers.
 *
 * Any addresses between 0x50 and 0xff not listed below are reserved and
 * should not be touched.
 */

#define GMCHCFG	0x50		/* GMCH Configuration */
#define PAMR	0x51		/* Programmable Attributes */
#define DRP	0x52		/* DRAM Row Population */
#define DRAMT	0x53		/* DRAM Timing */
#define FDHC	0x58		/* Fixed DRAM Hole Control */
#define SMRAM	0x70		/* System Management RAM Control */
#define MISSC	0x72		/* Miscellaneous Control */
#define MISSC2	0x80		/* Miscellaneous Control 2 */
#define BUFF_SC	0x92		/* System Memory Buffer Strength Control */

int smbus_read_byte(u8 device, u8 address);

#endif
