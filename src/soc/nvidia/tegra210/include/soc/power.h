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

#ifndef __SOC_NVIDIA_TEGRA210_POWER_H__
#define __SOC_NVIDIA_TEGRA210_POWER_H__

#include <soc/pmc.h>

void power_ungate_partition(uint32_t id);
void power_gate_partition(uint32_t id);

uint8_t pmc_rst_status(void);
void pmc_print_rst_status(void);
void remove_clamps(int id);
void pmc_override_pwr_det(uint32_t bits, uint32_t override);

#endif	/* __SOC_NVIDIA_TEGRA210_POWER_H__ */
