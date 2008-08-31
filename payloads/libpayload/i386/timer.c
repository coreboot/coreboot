/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

/**
 * @file i386/timer.c
 * i386 specific timer routines
 */

#include <libpayload.h>
#include <arch/rdtsc.h>

/**
 * @ingroup arch
 * Global variable containing the speed of the processor in KHz.
 */
u32 cpu_khz;

/**
 * Calculate the speed of the processor for use in delays.
 *
 * @return The CPU speed in kHz.
 */
unsigned int get_cpu_speed(void)
{
	unsigned long long start, end;

	/* Set up the PPC port - disable the speaker, enable the T2 gate. */
	outb((inb(0x61) & ~0x02) | 0x01, 0x61);

	/* Set the PIT to Mode 0, counter 2, word access. */
	outb(0xB0, 0x43);

	/* Load the counter with 0xffff. */
	outb(0xff, 0x42);
	outb(0xff, 0x42);

	/* Read the number of ticks during the period. */
	start = rdtsc();
	while (!(inb(0x61) & 0x20)) ;
	end = rdtsc();

	/*
	 * The clock rate is 1193180 Hz, the number of milliseconds for a
	 * period of 0xffff is 1193180 / (0xFFFF * 1000) or .0182.
	 * Multiply that by the number of measured clocks to get the kHz value.
	 */
	cpu_khz = (unsigned int)((end - start) * 1193180U / (1000 * 0xffff));

	return cpu_khz;
}

static inline void _delay(unsigned int delta)
{
	unsigned long long timeout = rdtsc() + delta;
	while (rdtsc() < timeout) ;
}

/**
 * Delay for a specified number of nanoseconds.
 *
 * @param n Number of nanoseconds to delay for.
 */
void ndelay(unsigned int n)
{
	_delay(n * cpu_khz / 1000000);
}

/**
 * Delay for a specified number of microseconds.
 *
 * @param n Number of microseconds to delay for.
 */
void udelay(unsigned int n)
{
	_delay(n * cpu_khz / 1000);
}

/**
 * Delay for a specified number of milliseconds.
 *
 * @param m Number of milliseconds to delay for.
 */
void mdelay(unsigned int m)
{
	_delay(m * cpu_khz);
}

/**
 * Delay for a specified number of seconds.
 *
 * @param s Number of seconds to delay for.
 */
void delay(unsigned int s)
{
	_delay(s * cpu_khz * 1000);
}
