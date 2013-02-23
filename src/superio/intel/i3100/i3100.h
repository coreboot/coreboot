/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
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

#ifndef SUPERIO_INTEL_I3100_I3100_H
#define SUPERIO_INTEL_I3100_I3100_H

/*
 * Datasheet:
 *  - Name: Intel 3100 Chipset
 *  - URL: http://www.intel.com/design/intarch/datashts/313458.htm
 *  - PDF: http://download.intel.com/design/intarch/datashts/31345803.pdf
 *  - Revision / Date: 007, October 2008
 *  - Order number: 313458-007US
 */

/*
 * The SIW ("Serial I/O and Watchdog Timer") integrated into the i3100 is
 * very similar to a Super I/O, both in functionality and config mechanism.
 *
 * The SIW contains:
 *  - UART(s)
 *  - Serial interrupt controller
 *  - Watchdog timer (WDT)
 *  - LPC interface
 */

/* Logical device numbers (LDNs). */
#define I3100_SP1 0x04 /* Com1 */
#define I3100_SP2 0x05 /* Com2 */
#define I3100_WDT 0x06 /* Watchdog timer */

/* Registers and bit definitions: */

#define I3100_SIW_CONFIGURATION		0x29

/*
 * SIW_CONFIGURATION[3:2] = UART_CLK predivide
 * 00: divide by 1
 * 01: divide by 8
 * 10: divide by 26
 * 11: reserved
 */
#define I3100_UART_CLK_PREDIVIDE_1	0x00
#define I3100_UART_CLK_PREDIVIDE_8	0x01
#define I3100_UART_CLK_PREDIVIDE_26	0x02

#endif
