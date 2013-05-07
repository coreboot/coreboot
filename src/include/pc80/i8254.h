/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
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

#ifndef PC80_I8254_H
#define PC80_I8254_H

/* Ports for the 8254 timer chip */
#define TIMER0_PORT	0x40
#define TIMER1_PORT	0x41
#define TIMER2_PORT     0x42
#define TIMER_MODE_PORT 0x43

/* Meaning of the mode bits */
#define TIMER0_SEL      0x00
#define TIMER1_SEL      0x40
#define TIMER2_SEL      0x80
#define READBACK_SEL    0xC0

#define LATCH_COUNT     0x00
#define LOBYTE_ACCESS   0x10
#define HIBYTE_ACCESS   0x20
#define WORD_ACCESS     0x30

#define MODE0           0x00
#define MODE1           0x02
#define MODE2           0x04
#define MODE3           0x06
#define MODE4           0x08
#define MODE5           0x0A

#define BINARY_COUNT    0x00
#define BCD_COUNT       0x01

/* Timers tick over at this rate */
#define TICKS_PER_MS    1193

/* Parallel Peripheral Controller Port B */
#define PPC_PORTB       0x61

/* Meaning of the port bits */
#define PPCB_T2OUT      0x20    /* Bit 5 */
#define PPCB_SPKR       0x02    /* Bit 1 */
#define PPCB_T2GATE     0x01    /* Bit 0 */

void setup_i8254(void);
#endif
