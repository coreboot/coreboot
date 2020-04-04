/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __SOC_NVIDIA_TEGRA124_INCLUDE_SOC_DISPLAY_H__
#define __SOC_NVIDIA_TEGRA124_INCLUDE_SOC_DISPLAY_H__

#define COLOR_WHITE	0xFFFFFF

struct soc_nvidia_tegra124_config;	/* forward declaration */
void setup_display(struct soc_nvidia_tegra124_config *config);

#define FB_SIZE_MB (32)

#endif /* __SOC_NVIDIA_TEGRA124_INCLUDE_SOC_DISPLAY_H__ */
