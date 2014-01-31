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

#ifndef _BAYTRAIL_LPC_H_
#define _BAYTRAIL_LPC_H_

/* PCI config registers in LPC bridge. */
#define REVID		0x08
#define ABASE		0x40
#define PBASE		0x44
#define GBASE		0x48
#define IOBASE		0x4c
#define IBASE		0x50
#define SBASE		0x54
#define MPBASE		0x58
#define UART_CONT	0x80
#define RCBA		0xf0


#define RID_A_STEPPING_START 1
#define RID_B_STEPPING_START 5
enum baytrail_stepping {
	STEP_A0,
	STEP_A1,
	STEP_B0,
	STEP_B1,
	STEP_B2,
	STEP_B3,
};

#endif /* _BAYTRAIL_LPC_H_ */
