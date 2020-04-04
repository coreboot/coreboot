/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/mmio.h>
#include <soc/addressmap.h>

#include "pingroup.h"

static uint32_t *pingroup_regs = (void *)TEGRA_APB_PINGROUP_BASE;

void pingroup_set_config(int group_index, uint32_t config)
{
	write32(&pingroup_regs[group_index], config);
}

uint32_t pingroup_get_config(int group_index)
{
	return read32(&pingroup_regs[group_index]);
}
