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
 */

#include <arch/io.h>
#include <pc80/i8254.h>

/* Initialize i8254 timers */

void setup_i8254(void)
{
	/* Timer 0 (taken from biosemu) */
	outb(TIMER0_SEL | WORD_ACCESS | MODE3 | BINARY_COUNT, TIMER_MODE_PORT);
	outb(0x00, TIMER0_PORT);
	outb(0x00, TIMER0_PORT);

	/* Timer 1 */
	outb(TIMER1_SEL | LOBYTE_ACCESS | MODE3 | BINARY_COUNT,
	     TIMER_MODE_PORT);
	outb(0x12, TIMER1_PORT);
}

#if IS_ENABLED(CONFIG_UDELAY_TIMER2)
static void load_timer2(unsigned int ticks)
{
	/* Set up the timer gate, turn off the speaker */
	outb((inb(PPC_PORTB) & ~PPCB_SPKR) | PPCB_T2GATE, PPC_PORTB);
	outb(TIMER2_SEL | WORD_ACCESS | MODE0 | BINARY_COUNT, TIMER_MODE_PORT);
	outb(ticks & 0xFF, TIMER2_PORT);
	outb(ticks >> 8, TIMER2_PORT);
}

void udelay(int usecs)
{
	load_timer2((usecs * TICKS_PER_MS) / 1000);
	while ((inb(PPC_PORTB) & PPCB_T2OUT) == 0)
		;
}
#endif
