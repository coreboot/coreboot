/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <console/console.h>
#include <soc/msr.h>
#include <types.h>

#define PSTATE_DEF_FREQ_DIV_MIN		0x8
#define PSTATE_DEF_EIGHTH_STEP_MAX	0x1A
#define PSTATE_DEF_FREQ_DIV_MAX		0x3E
#define PSTATE_DEF_CORE_FREQ_BASE	25

uint32_t get_pstate_core_freq(union pstate_msr pstate_reg)
{
	uint32_t core_freq, core_freq_mul, core_freq_div;
	bool valid_freq_divisor;

	/* Core frequency multiplier */
	core_freq_mul = pstate_reg.cpu_fid_0_7;

	/* Core frequency divisor ID */
	core_freq_div = pstate_reg.cpu_dfs_id;

	if (core_freq_div == 0) {
		return 0;
	} else if ((core_freq_div >= PSTATE_DEF_FREQ_DIV_MIN)
		   && (core_freq_div <= PSTATE_DEF_EIGHTH_STEP_MAX)) {
		/* Allow 1/8 integer steps for this range */
		valid_freq_divisor = true;
	} else if ((core_freq_div > PSTATE_DEF_EIGHTH_STEP_MAX)
		   && (core_freq_div <= PSTATE_DEF_FREQ_DIV_MAX) && !(core_freq_div & 0x1)) {
		/* Only allow 1/4 integer steps for this range */
		valid_freq_divisor = true;
	} else {
		valid_freq_divisor = false;
	}

	if (valid_freq_divisor) {
		/* 25 * core_freq_mul / (core_freq_div / 8) */
		core_freq =
			((PSTATE_DEF_CORE_FREQ_BASE * core_freq_mul * 8) / (core_freq_div));
	} else {
		printk(BIOS_WARNING, "Undefined core_freq_div %x used. Force to 1.\n",
		       core_freq_div);
		core_freq = (PSTATE_DEF_CORE_FREQ_BASE * core_freq_mul);
	}
	return core_freq;
}
