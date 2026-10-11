/* SPDX-License-Identifier: GPL-2.0-only */

#include "atf.h"
#include <armv8/arch/smc.h>
#include <stdint.h>

#define MM_COMM_BUFFER 0x88300000

uint32_t atf_mm_version(void)
{
	return smc_call0(MM_VERSION);
}

uint32_t atf_mm_communicate(void)
{
	return smc_call2(MM_COMMUNICATE, 0, MM_COMM_BUFFER);
}
