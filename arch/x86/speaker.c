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
 */

#include <io.h>
#include <lib.h>

#define I82C54_CONTROL_WORD_REGISTER	0x43	/* Write-only. */

#define I82C54_COUNTER0			0x40
#define I82C54_COUNTER1			0x41
#define I82C54_COUNTER2			0x42

#define PC_SPEAKER_PORT			0x61

/**
 * Use the PC speaker to create a tone/sound of the specified frequency.
 *
 * The Intel 82C54 CHMOS Programmable Interval Timer (PIT) provides a
 * superset of the functions of the original Intel 8253/8254 PIT. It has
 * three programmable counters/timers (counter 0, 1, and 2). Counter 2 can
 * be used to generate tones/sounds of various frequencies and duration.
 *
 * See also:
 *  - http://en.wikipedia.org/wiki/Pc_speaker
 *  - http://en.wikipedia.org/wiki/Intel_8253
 *
 * @param freq The frequency of the tone.
 */
void speaker_enable(u16 freq)
{
	/* Select counter 2. Read/write LSB first, then MSB. Use mode 3
	   (square wave generator). Use a 16bit binary counter. */
	outb(0xb6, I82C54_CONTROL_WORD_REGISTER);

	/* Set the desired tone frequency. */
	outb((u8)(freq & 0x00ff), I82C54_COUNTER2);	/* LSB. */
	outb((u8)(freq >> 8), I82C54_COUNTER2);		/* MSB. */

	/* Enable the PC speaker (set bits 0 and 1). */
	outb(inb(PC_SPEAKER_PORT) | 0x03, PC_SPEAKER_PORT);
}

/**
 * Disable the PC speaker.
 */
void speaker_disable(void)
{
	/* Disable the PC speaker (clear bits 0 and 1). */
	outb(inb(PC_SPEAKER_PORT) & 0xfc, PC_SPEAKER_PORT);
}

/**
 * Use the PC speaker to create a tone/beep of the specified frequency
 * and duration.
 *
 * Wait for a short amount of time after the beep to make it distinguishable
 * from the next beep (if any).
 *
 * @param freq The frequency of the tone/beep.
 * @param duration The duration of the tone/beep in milliseconds.
 */
void speaker_tone(u16 freq, unsigned int duration)
{
	speaker_enable(freq);
	mdelay(duration);
	speaker_disable();
	delay(10);
}

/**
 * Use the PC speaker to create a short tone/beep.
 */
void beep_short(void)
{
#if defined(CONFIG_BEEPS) && (CONFIG_BEEPS == 1)
	speaker_tone(1760, 500);	/* 1760 == note A6. */
#endif
}

/**
 * Use the PC speaker to create a long tone/beep.
 */
void beep_long(void)
{
#if defined(CONFIG_BEEPS) && (CONFIG_BEEPS == 1)
	speaker_tone(1760, 2000);	/* 1760 == note A6. */
#endif
}

