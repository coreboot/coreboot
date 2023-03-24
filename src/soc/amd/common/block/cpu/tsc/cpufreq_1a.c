/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <soc/msr.h>
#include <types.h>

#define PSTATE_DEF_CORE_FREQ_BASE	5

uint32_t get_pstate_core_freq(union pstate_msr pstate_reg)
{
	uint32_t core_freq_mul;

	/* Core frequency multiplier */
	core_freq_mul = pstate_reg.cpu_fid_0_11;

	/* CPU frequency is 5 * core_freq_mul */
	return PSTATE_DEF_CORE_FREQ_BASE * core_freq_mul;
}
