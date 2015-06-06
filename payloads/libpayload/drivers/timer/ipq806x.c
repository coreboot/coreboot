/*
 * This file is part of the depthcharge project.
 *
 * Copyright (C) 2014 The Linux Foundation. All rights reserved.
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

/*
 * TODO(vbendeb): reverted this hack once proper timer code is in place (see
 * http://crosbug.com/p/28880 for details.
 */
#define MIN_TIMER_FREQ 1000000

uint64_t timer_hz(void)
{
	return (CONFIG_LP_IPQ806X_TIMER_FREQ >= MIN_TIMER_FREQ) ?
		CONFIG_LP_IPQ806X_TIMER_FREQ : MIN_TIMER_FREQ;
}

uint64_t timer_raw_value(void)
{
	uint64_t rawv = readl((void *)CONFIG_LP_IPQ806X_TIMER_REG);

	/*
	 * This is extremely crude, but it kicks in only for the case when the
	 * timer clock frequency is below 1MHz, which should never be the case
	 * on a properly configured system. The compiler will eliminate the
	 * check as long as config value exceeds 1MHz.
	 */
	if (CONFIG_LP_IPQ806X_TIMER_FREQ < MIN_TIMER_FREQ)
		rawv *= (MIN_TIMER_FREQ / CONFIG_LP_IPQ806X_TIMER_FREQ);

	return rawv;
}
