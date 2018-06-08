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

#ifndef __SOC_NVIDIA_TEGRA124_INCLUDE_SOC_DISPLAY_H__
#define __SOC_NVIDIA_TEGRA124_INCLUDE_SOC_DISPLAY_H__

#define COLOR_WHITE	0xFFFFFF

struct soc_nvidia_tegra124_config;	/* forward declaration */
void setup_display(struct soc_nvidia_tegra124_config *config);

#define FB_SIZE_MB (32)

#endif /* __SOC_NVIDIA_TEGRA124_INCLUDE_SOC_DISPLAY_H__ */
