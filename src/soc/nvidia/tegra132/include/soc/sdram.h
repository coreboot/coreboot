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

#ifndef __SOC_NVIDIA_TEGRA132_SDRAM_H__
#define __SOC_NVIDIA_TEGRA132_SDRAM_H__

#include <soc/sdram_param.h>

uint32_t sdram_get_ram_code(void);
void sdram_init(const struct sdram_params *param);

/* Save params to PMC scratch registers for use by BootROM on LP0 resume. */
void sdram_lp0_save_params(const struct sdram_params *sdram);

#endif /* __SOC_NVIDIA_TEGRA132_SDRAM_H__ */
