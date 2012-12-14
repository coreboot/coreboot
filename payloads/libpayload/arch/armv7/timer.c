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
 * @file armv7/timer.c
 * ARMv7 specific timer routines
 */

#include <libpayload.h>

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
	/* FIXME */
	cpu_khz = 1000000U;

	return cpu_khz;
}

static inline void _delay(unsigned long long delta)
{
	/* FIXME */
}

/**
 * Delay for a specified number of nanoseconds.
 *
 * @param n Number of nanoseconds to delay for.
 */
void ndelay(unsigned int n)
{
	_delay((unsigned long long)n * cpu_khz / 1000000);
}

/**
 * Delay for a specified number of microseconds.
 *
 * @param n Number of microseconds to delay for.
 */
void udelay(unsigned int n)
{
	_delay((unsigned long long)n * cpu_khz / 1000);
}

/**
 * Delay for a specified number of milliseconds.
 *
 * @param m Number of milliseconds to delay for.
 */
void mdelay(unsigned int m)
{
	_delay((unsigned long long)m * cpu_khz);
}

/**
 * Delay for a specified number of seconds.
 *
 * @param s Number of seconds to delay for.
 */
void delay(unsigned int s)
{
	int i;
	for (i=0; i<1000; i++)
		_delay((unsigned long long)s * cpu_khz);
}
