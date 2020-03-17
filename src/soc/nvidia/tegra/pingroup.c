/*
 * This file is part of the coreboot project.
 *
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
