/*
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SOC_NVIDIA_TEGRA124_INCLUDE_SOC_DISPLAY_H__
#define __SOC_NVIDIA_TEGRA124_INCLUDE_SOC_DISPLAY_H__

#define COLOR_WHITE	0xFFFFFF

struct soc_nvidia_tegra124_config;	/* forward declaration */
void setup_display(struct soc_nvidia_tegra124_config *config);

#define FB_SIZE_MB (32)

#endif /* __SOC_NVIDIA_TEGRA124_INCLUDE_SOC_DISPLAY_H__ */
