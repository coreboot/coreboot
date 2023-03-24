/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <console/console.h>
#include <soc/msr.h>
#include <types.h>

#define PSTATE_DEF_FREQ_DIV_MAX		4
#define PSTATE_DEF_CORE_FREQ_BASE	100
#define PSTATE_DEF_CORE_FREQ_ID_OFFSET	0x10

uint32_t get_pstate_core_freq(union pstate_msr pstate_reg)
{
	uint32_t core_freq, core_freq_mul, core_freq_div;

	/* Core frequency multiplier */
	core_freq_mul = pstate_reg.cpu_fid_0_5;

	/* Core frequency divisor ID */
	core_freq_div = pstate_reg.cpu_dfs_id;

	if (core_freq_div > PSTATE_DEF_FREQ_DIV_MAX) {
		printk(BIOS_WARNING, "Undefined core_freq_div %x used. Force to lowest "
				     "divider.\n", core_freq_div);
		core_freq_div = 0;
	}

	core_freq = (PSTATE_DEF_CORE_FREQ_BASE *
		(core_freq_mul + PSTATE_DEF_CORE_FREQ_ID_OFFSET)) / (1 << core_freq_div);

	return core_freq;
}
