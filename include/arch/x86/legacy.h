/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/*
 * Datasheet:
 *  - Name: 82C54 CHMOS Programmable Interval Timer
 *  - PDF: http://www.intel.com/design/archives/periphrl/docs/23124406.pdf
 *  - Order number: 231244-006
 *
 * See also:
 *  - http://en.wikipedia.org/wiki/Intel_8253
 */

#define I82C54_CONTROL_WORD_REGISTER	0x43	/* Write-only! */
#define I82C54_COUNTER0			0x40
#define I82C54_COUNTER1			0x41
#define I82C54_COUNTER2			0x42

void setup_i8259(void);
void uart_init(void);
void rtc_init(int invalid);
void isa_dma_init(void);
