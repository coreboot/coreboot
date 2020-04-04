/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/sdram.h>
#include <soc/nvidia/tegra/apbmisc.h>

uint32_t sdram_get_ram_code(void)
{
	struct apbmisc *misc = (struct apbmisc *)TEGRA_APB_MISC_BASE;

	return (read32(&misc->pp_strapping_opt_a) &
		PP_STRAPPING_OPT_A_RAM_CODE_MASK) >>
		PP_STRAPPING_OPT_A_RAM_CODE_SHIFT;
}
