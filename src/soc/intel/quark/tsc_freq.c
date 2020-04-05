/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <cpu/x86/tsc.h>

unsigned long tsc_freq_mhz(void)
{
	/* CPU freq = 400 MHz */
	return 400;
}
