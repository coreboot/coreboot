/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/msr.h>
#include "model_206ax.h"

int get_platform_id(void)
{
	msr_t msr;

	msr = rdmsr(IA32_PLATFORM_ID);
	/* Read Platform Id Bits 52:50 */
	return (msr.hi >> 18) & 0x7;
}
