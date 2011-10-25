/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Datasheet:
 *  - Name: 82C54 CHMOS Programmable Interval Timer
 *  - PDF: http://www.intel.com/design/archives/periphrl/docs/23124406.pdf
 *  - Order number: 231244-006
 */

#include <libpayload.h>

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
	u16 reg16 = 1193180 / freq;

	/* Select counter 2. Read/write LSB first, then MSB. Use mode 3
	   (square wave generator). Use a 16bit binary counter. */
	outb(0xb6, I82C54_CONTROL_WORD_REGISTER);

	/* Set the desired tone frequency. */
	outb((u8)(reg16 & 0x00ff), I82C54_COUNTER2);	/* LSB. */
	outb((u8)(reg16 >> 8), I82C54_COUNTER2);	/* MSB. */

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
	mdelay(100);
}
