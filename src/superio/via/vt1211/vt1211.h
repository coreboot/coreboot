/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Nick Barker <nick.barker9@btinternet.com>
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

#ifndef SUPERIO_VIA_VT1211_VT1211_H
#define SUPERIO_VIA_VT1211_VT1211_H

/* Logical device numbers (LDNs). */
#define VT1211_FDC		0x00	/* Floppy */
#define VT1211_PP		0x01	/* Parallel port */
#define VT1211_SP1		0x02	/* COM1 */
#define VT1211_SP2		0x03	/* COM2 */
#define VT1211_MIDI		0x06	/* MIDI */
#define VT1211_GAME		0x07	/* Game port (GMP) */
#define VT1211_GPIO		0x08	/* GPIO */
#define VT1211_WDG		0x09	/* Watchdog timer (WDG) */
#define VT1211_WUC		0x0a	/* Wake-up control (WUC) */
#define VT1211_HWM		0x0b	/* Hardware monitor (HM) */
#define VT1211_FIR		0x0c	/* Very fast IR (VFIR/FIR) */
#define VT1211_ROM		0x0d	/* Flash ROM */

#endif
