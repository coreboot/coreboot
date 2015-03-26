/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <soc/addressmap.h>

#include "pinmux.h"

static uint32_t *pinmux_regs = (void *)TEGRA_APB_PINMUX_BASE;

void pinmux_set_config(int pin_index, uint32_t config)
{
	write32(&pinmux_regs[pin_index], config);
}

uint32_t pinmux_get_config(int pin_index)
{
	return read32(&pinmux_regs[pin_index]);
}
