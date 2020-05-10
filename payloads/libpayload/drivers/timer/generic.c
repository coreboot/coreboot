/*
 *
 * Copyright 2016 Google Inc.
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
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
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

#include <assert.h>
#include <libpayload.h>

uint64_t timer_hz(void)
{
	/* libc/time.c currently requires all timers to be at least 1MHz. */
	assert(CONFIG_LP_TIMER_GENERIC_HZ >= 1000000);
	return CONFIG_LP_TIMER_GENERIC_HZ;
}

uint64_t timer_raw_value(void)
{
	uint64_t cur_tick;
	uint32_t count_h;
	uint32_t count_l;

	if (!CONFIG_LP_TIMER_GENERIC_HIGH_REG)
		return readl(phys_to_virt(CONFIG_LP_TIMER_GENERIC_REG));

	do {
		count_h = readl(phys_to_virt(CONFIG_LP_TIMER_GENERIC_HIGH_REG));
		count_l = readl(phys_to_virt(CONFIG_LP_TIMER_GENERIC_REG));
		cur_tick = readl(phys_to_virt(CONFIG_LP_TIMER_GENERIC_HIGH_REG));
	} while (cur_tick != count_h);

	return (cur_tick << 32) + count_l;
}
