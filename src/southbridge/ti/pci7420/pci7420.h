/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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

		     // 0844d060 (old)
#define SYSCTL	0x80 // 08405061
#define   RIMUX		(1 << 0)

#define GENCTL	0x86
#define   FW1394_PRIO	(0 << 0)
#define   CARDBUS_PRIO	(1 << 0)
#define   FLASH_PRIO	(2 << 0)
#define   ROUNDR_PRIO	(3 << 0)
#define   DISABLE_OHCI  (1 << 3)
#define   DISABLE_SKTB	(1 << 4)
#define   DISABLE_FM	(1 << 5)
#define   P12V_SW_SEL	(1 << 10)

#define MFUNC	0x8c

#define CARDCTL	0x91
#define   SPKROUTEN	(1 << 1)

#define DEVCTL	0x92
#define   INT_MODE_PAR	(0 << 1)
#define   INT_MODE_RSV  (1 << 1)
#define   INT_MODE_MIX  (2 << 1)
#define   INT_MODE_SER  (3 << 1)

#define INTA 0
#define INTB 1
#define INTC 2
#define INTD 3

