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

#ifndef __SOC_NVIDIA_TEGRA210_SOC_ROMSTAGE_H__
#define __SOC_NVIDIA_TEGRA210_SOC_ROMSTAGE_H__

void romstage(void);
void romstage_mainboard_init(void);

void mainboard_configure_pmc(void);
void mainboard_enable_vdd_cpu(void);

#endif /* __SOC_NVIDIA_TEGRA210_SOC_ROMSTAGE_H__ */
