/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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
 */

#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/lxdef.h>
#include "northbridge.h"

void lx_pll_reset(void)
{
	msr_t msrGlcpSysRstpll;

	msrGlcpSysRstpll = rdmsr(GLCP_SYS_RSTPLL);

	printk(BIOS_DEBUG, "MSR GLCP_SYS_RSTPLL (%08x) value is %08x:%08x\n",
		GLCP_SYS_RSTPLL, msrGlcpSysRstpll.hi, msrGlcpSysRstpll.lo);

	post_code(POST_PLL_INIT);

	if (!(msrGlcpSysRstpll.lo & (1 << RSTPLL_LOWER_SWFLAGS_SHIFT))) {
		printk(BIOS_DEBUG, "Configuring PLL.\n");
		if (CONFIG_PLL_MANUAL_CONFIG) {
			post_code(POST_PLL_MANUAL);
			/* CPU and GLIU mult/div (GLMC_CLK = GLIU_CLK / 2)  */
			msrGlcpSysRstpll.hi = CONFIG_PLLMSRhi;

			/* Hold Count - how long we will sit in reset */
			msrGlcpSysRstpll.lo = CONFIG_PLLMSRlo;
		} else {
			/*automatic configuration (straps) */
			post_code(POST_PLL_STRAP);
			msrGlcpSysRstpll.lo &=
			    ~(0xFF << RSTPPL_LOWER_HOLD_COUNT_SHIFT);
			msrGlcpSysRstpll.lo |=
			    (0xDE << RSTPPL_LOWER_HOLD_COUNT_SHIFT);
			msrGlcpSysRstpll.lo &=
			    ~(RSTPPL_LOWER_COREBYPASS_SET |
			      RSTPPL_LOWER_MBBYPASS_SET);
			msrGlcpSysRstpll.lo |=
			    RSTPPL_LOWER_COREPD_SET | RSTPPL_LOWER_CLPD_SET;
		}
		/* Use SWFLAGS to remember: "we've already been here"  */
		msrGlcpSysRstpll.lo |= (1 << RSTPLL_LOWER_SWFLAGS_SHIFT);

		/* "reset the chip" value */
		msrGlcpSysRstpll.lo |= RSTPPL_LOWER_CHIP_RESET_SET;
		wrmsr(GLCP_SYS_RSTPLL, msrGlcpSysRstpll);

		/* You should never get here..... The chip has reset. */
		post_code(POST_PLL_RESET_FAIL);
		die("CONFIGURING PLL FAILURE\n");

	}
	printk(BIOS_DEBUG, "PLL configured.\n");
	return;
}

unsigned int GeodeLinkSpeed(void)
{
	unsigned int speed;
	msr_t msr;

	msr = rdmsr(GLCP_SYS_RSTPLL);
	speed = ((((msr.hi >> RSTPLL_UPPER_GLMULT_SHIFT) & 0x1F) + 1) * 333) / 10;
	if ((((((msr.hi >> RSTPLL_UPPER_GLMULT_SHIFT) & 0x1F) + 1) * 333) % 10) > 5) {
		++speed;
	}
	return (speed);
}
