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
 * @file x86/timer.c
 * x86 specific timer routines
 */

#include <libpayload.h>
#include <arch/rdtsc.h>

/**
 * @ingroup arch
 * Global variable containing the speed of the processor in KHz.
 */
uint32_t cpu_khz;

/**
 * Calculate the speed of the processor for use in delays.
 *
 * @return The CPU speed in kHz.
 */
unsigned int get_cpu_speed(void)
{
	unsigned long long start, end;
	const uint32_t clock_rate = 1193182; // 1.193182 MHz
	const uint16_t interval = (2 * clock_rate) / 1000; // 2 ms

	/* Set up the PPC port - disable the speaker, enable the T2 gate. */
	outb((inb(0x61) & ~0x02) | 0x01, 0x61);

	/* Set the PIT to Mode 0, counter 2, word access. */
	outb(0xB0, 0x43);

	/* Load the interval into the counter. */
	outb(interval & 0xff, 0x42);
	outb((interval >> 8) & 0xff, 0x42);

	/* Read the number of ticks during the period. */
	start = rdtsc();
	while (!(inb(0x61) & 0x20)) ;
	end = rdtsc();

	/*
	 * The number of milliseconds for a period is
	 * clock_rate / (interval * 1000). Multiply that by the number of
	 * measured clocks to get the kHz value.
	 */
	cpu_khz = (end - start) * clock_rate / (1000 * interval);

	return cpu_khz;
}

uint64_t timer_hz(void)
{
	return lib_sysinfo.cpu_khz * 1000;
}

uint64_t timer_raw_value(void)
{
	return rdtsc();
}
