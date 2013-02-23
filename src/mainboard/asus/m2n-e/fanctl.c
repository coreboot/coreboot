/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Uwe Hermann <uwe@hermann-uwe.de>
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

/*
 * The ASUS M2N-E has 6 different fans, connected to two different chips:
 * - ITE IT8716F: fan1 = CPU_FAN, fan2 = CHA_FAN1, fan3 = PWR_FAN1
 * - Analog Devices ADT7475: fan1 = CHA_FAN4, fan2 = CHA_FAN2, fan3 = CHA_FAN3
 */

#include <arch/io.h>
#include <stdlib.h>
#include <superio/ite/it8716f/it8716f.h>

static void write_index(u16 port, u8 reg, u8 value)
{
	outb(reg, port);
	outb(value, port + 1);
}

static const struct {
	u8 index;
	u8 value;
} sequence[] = {
	/* Enable startup of monitoring operations. */
	{ 0x00, 0x11},
	/* Polarity active-high, PWM frequency 23.43KHz, activate fans 1-3. */
	{ 0x14, 0xd7},
	/* Set the correct sensor types. TMPIN1: diode, TMPIN2/3: resistor. */
	{ 0x51, 0x31},
	/* Fan1 (CPU_FAN) is software-controlled. */
	{ 0x15, 0x7f},
	/* Fan2 (CHA_FAN1) is software-controlled. */
	{ 0x16, 0x7f},
	/* Fan3 (PWR_FAN1) is software-controlled. */
	{ 0x17, 0x7f},
	/* Enable fan1/2/3, select "on/off mode" for all of them. */
	{ 0x13, 0x70},
};

/* Called from src/ite/it8716f/superio.c. */
void init_ec(u16 base)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(sequence); i++)
		write_index(base, sequence[i].index, sequence[i].value);
}
