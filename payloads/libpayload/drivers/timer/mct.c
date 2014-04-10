/*
 * Copyright 2013 Google Inc.
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

#include <arch/io.h>
#include <libpayload.h>
#include <libpayload-config.h>
#include <stdint.h>

#include "config.h"

struct __attribute__((packed)) mct_regs
{
	uint32_t mct_cfg;
	uint8_t reserved0[0xfc];
	uint32_t g_cnt_l;
	uint32_t g_cnt_u;
	uint8_t reserved1[0x8];
	uint32_t g_cnt_wstat;
	uint8_t reserved2[0xec];
	uint32_t g_comp0_l;
	uint32_t g_comp0_u;
	uint32_t g_comp0_addr_incr;
	uint8_t reserved3[0x4];
	uint32_t g_comp1_l;
	uint32_t g_comp1_u;
	uint32_t g_comp1_addr_incr;
	uint8_t reserved4[0x4];
	uint32_t g_comp2_l;
	uint32_t g_comp2_u;
	uint32_t g_comp2_addr_incr;
	uint8_t reserved5[0x4];
	uint32_t g_comp3_l;
	uint32_t g_comp3_u;
	uint32_t g_comp3_addr_incr;
	uint8_t reserved6[0x4];
	uint32_t g_tcon;
	uint32_t g_int_cstat;
	uint32_t g_int_enb;
	uint32_t g_wstat;
	uint8_t reserved7[0xb0];
	uint32_t l0_tcntb;
	uint32_t l0_tcnto;
	uint32_t l0_icntb;
	uint32_t l0_icnto;
	uint32_t l0_frcntb;
	uint32_t l0_frcnto;
	uint8_t reserved8[0x8];
	uint32_t l0_tcon;
	uint8_t reserved9[0xc];
	uint32_t l0_int_cstat;
	uint32_t l0_int_enb;
	uint8_t reserved10[0x8];
	uint32_t l0_wstat;
	uint8_t reserved11[0xbc];
	uint32_t l1_tcntb;
	uint32_t l1_tcnto;
	uint32_t l1_icntb;
	uint32_t l1_icnto;
	uint32_t l1_frcntb;
	uint32_t l1_frcnto;
	uint8_t reserved12[0x8];
	uint32_t l1_tcon;
	uint8_t reserved13[0xc];
	uint32_t l1_int_cstat;
	uint32_t l1_int_enb;
	uint8_t reserved14[0x8];
	uint32_t l1_wstat;
};

uint64_t timer_hz(void)
{
	return CONFIG_LP_TIMER_MCT_HZ;
}

uint64_t timer_raw_value(void)
{
	static int enabled = 0;

	struct mct_regs * const mct =
		(struct mct_regs *)(uintptr_t)CONFIG_LP_TIMER_MCT_ADDRESS;

	if (!enabled) {
		writel(readl(&mct->g_tcon) | (0x1 << 8), &mct->g_tcon);
		enabled = 1;
	}

	uint64_t upper = readl(&mct->g_cnt_u);
	uint64_t lower = readl(&mct->g_cnt_l);

	return (upper << 32) | lower;
}
