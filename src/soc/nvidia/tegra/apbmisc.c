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
 */

#include <arch/io.h>
#include <soc/addressmap.h>

#include "apbmisc.h"

static struct apbmisc *misc = (struct apbmisc *)TEGRA_APB_MISC_BASE;

void enable_jtag(void)
{
	write32(&misc->pp_config_ctl, PP_CONFIG_CTL_JTAG);
}

void clamp_tristate_inputs(void)
{
	write32(&misc->pp_pinmux_global, PP_PINMUX_CLAMP_INPUTS);
}

void tegra_revision_info(struct tegra_revision *id)
{
	uintptr_t gp_hidrev= (uintptr_t)TEGRA_APB_MISC_BASE + MISC_GP_HIDREV;
	uint32_t reg;

	reg = read32((void *)(gp_hidrev));

	id->hid_fam = (reg >> 0) & 0x0f;
	id->chip_id = (reg >> 8) & 0xff;
	id->major = (reg >> 4) & 0x0f;
	id->minor = (reg >> 16) & 0x07;
}
