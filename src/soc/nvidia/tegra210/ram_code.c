/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <arch/io.h>
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
