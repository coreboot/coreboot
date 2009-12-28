/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Nick Barker <nick.barker9@btinternet.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/* vt1211 PNP devices */

#define VT1211_FDC		0	/* Floppy */
#define VT1211_PP		1	/* Parallel Port */
#define VT1211_SP1		2	/* COM1 */
#define VT1211_SP2		3	/* COM2 */
#define VT1211_MIDI		6	/* MIDI */
#define VT1211_GAME		7	/* Game port */
#define VT1211_GPIO		8	/* GPIO pins */
#define VT1211_WATCHDOG		9	/* Watchdog timer */
#define VT1211_WAKEUP		10	/* Wakeup control */
#define VT1211_HWM		11	/* Hardware monitor */
#define VT1211_FIR		12	/* Irda */
#define VT1211_ROM		13	/* ROM control */
