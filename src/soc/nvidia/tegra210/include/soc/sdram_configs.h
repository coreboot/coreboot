/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_NVIDIA_TEGRA210_SDRAM_CONFIGS_H__
#define __SOC_NVIDIA_TEGRA210_SDRAM_CONFIGS_H__

#include <soc/sdram.h>

/* Loads SDRAM configurations for current system. */
const struct sdram_params *get_sdram_config(void);

#endif  /* __SOC_NVIDIA_TEGRA210_SDRAM_CONFIGS_H__ */
