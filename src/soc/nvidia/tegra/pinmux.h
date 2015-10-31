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
 */

#ifndef __SOC_NVIDIA_TEGRA_PINMUX_H__
#define __SOC_NVIDIA_TEGRA_PINMUX_H__

#include <stdint.h>

void pinmux_set_config(int pin_index, uint32_t config);
uint32_t pinmux_get_config(int pin_index);

#endif	/* __SOC_NVIDIA_TEGRA_PINMUX_H__ */
