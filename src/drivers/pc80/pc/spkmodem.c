/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <console/spkmodem.h>
#include <cpu/x86/tsc.h>

#define SPEAKER_PIT_FREQUENCY		0x1234dd


enum {
	PIT_COUNTER_0 = 0x40,
	PIT_COUNTER_1 = 0x41,
	PIT_COUNTER_2 = 0x42,
	PIT_CTRL = 0x43,
	PIT_SPEAKER_PORT = 0x61,
};


enum {
	PIT_SPK_TMR2 = 0x01,
	PIT_SPK_DATA = 0x02,
	PIT_SPK_TMR2_LATCH = 0x20
};

enum {
	PIT_CTRL_SELECT_MASK = 0xc0,
	PIT_CTRL_SELECT_0 = 0x00,
	PIT_CTRL_SELECT_1 = 0x40,
	PIT_CTRL_SELECT_2 = 0x80,

	PIT_CTRL_READLOAD_MASK = 0x30,
	PIT_CTRL_COUNTER_LATCH = 0x00,
	PIT_CTRL_READLOAD_LSB = 0x10,
	PIT_CTRL_READLOAD_MSB = 0x20,
	PIT_CTRL_READLOAD_WORD = 0x30,

	PIT_CTRL_MODE_MASK = 0x0e,
	PIT_CTRL_INTR_ON_TERM = 0x00,
	PIT_CTRL_PROGR_ONE_SHOT = 0x02,

	PIT_CTRL_RATE_GEN = 0x04,

	PIT_CTRL_SQUAREWAVE_GEN = 0x06,
	PIT_CTRL_SOFTSTROBE = 0x08,

	PIT_CTRL_HARDSTROBE = 0x0a,


	PIT_CTRL_COUNT_MASK = 0x01,
	PIT_CTRL_COUNT_BINARY = 0x00,
	PIT_CTRL_COUNT_BCD = 0x01
};


static void
make_tone(uint16_t freq_count, unsigned int duration)
{
	outb(PIT_CTRL_SELECT_2
		   | PIT_CTRL_READLOAD_WORD
		   | PIT_CTRL_SQUAREWAVE_GEN
		   | PIT_CTRL_COUNT_BINARY, PIT_CTRL);

	outb(freq_count & 0xff, PIT_COUNTER_2);

	outb((freq_count >> 8) & 0xff, PIT_COUNTER_2);

	outb(inb(PIT_SPEAKER_PORT)
		   | PIT_SPK_TMR2 | PIT_SPK_DATA,
		   PIT_SPEAKER_PORT);

	for (; duration; duration--) {
		unsigned short counter, previous_counter = 0xffff;

		while (1) {
			counter = inb(PIT_COUNTER_2);
			counter |= ((uint16_t)inb(PIT_COUNTER_2)) << 8;
			if (counter > previous_counter) {
				previous_counter = counter;
				break;
			}
			previous_counter = counter;
		}
	}
}

void spkmodem_tx_byte(unsigned char c)
{
	int i;

	make_tone(SPEAKER_PIT_FREQUENCY / 200, 4);
	for (i = 7; i >= 0; i--) {
		if ((c >> i) & 1)
			make_tone(SPEAKER_PIT_FREQUENCY / 2000, 20);
		else
			make_tone(SPEAKER_PIT_FREQUENCY / 4000, 40);
		make_tone(SPEAKER_PIT_FREQUENCY / 1000, 10);
	}
	make_tone(SPEAKER_PIT_FREQUENCY / 200, 0);
}

void spkmodem_init(void)
{
	/* Some cards need time to come online.
	 * Output some message to get it started.
	 */
	spkmodem_tx_byte('S');
	spkmodem_tx_byte('P');
	spkmodem_tx_byte('K');
	spkmodem_tx_byte('\r');
	spkmodem_tx_byte('\n');
}
