/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2018 Google LLC.
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

#include <libpayload.h>

/* The pause instruction can delay 10-140 CPU cycles, so avoid calling it when
 * getting close to finishing. Depending on the timer source, the timer can be
 * running at CPU frequency, bus frequency, or some arbitrary value. We assume
 * that the timer is running at the CPU frequency. */
#define PAUSE_THRESHOLD_TICKS		150

void arch_ndelay(uint64_t ns)
{
	uint64_t delta = ns * timer_hz() / NSECS_PER_SEC;
	uint64_t pause_delta = 0;
	uint64_t start = timer_raw_value();

	if (delta > PAUSE_THRESHOLD_TICKS)
		pause_delta = delta - PAUSE_THRESHOLD_TICKS;

	while (timer_raw_value() - start < pause_delta)
		asm volatile("pause\n\t");

	while (timer_raw_value() - start < delta)
		continue;
}
