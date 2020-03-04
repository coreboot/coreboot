/* SPDX-License-Identifier: BSD-3-Clause */
/* This file is part of the coreboot project. */

#include <stdint.h>
#include <arch/clock.h>

void set_cntfrq(uint32_t freq)
{
	__asm__ __volatile__("mcr p15, 0, %0, c14, c0, 0\n" :: "r"(freq));
}
