/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_NVIDIA_TEGRA210_SDRAM_H__
#define __SOC_NVIDIA_TEGRA210_SDRAM_H__

#include <soc/sdram_param.h>

uint32_t sdram_get_ram_code(void);
void sdram_init(const struct sdram_params *param);

/* Save params to PMC scratch registers for use by BootROM on LP0 resume. */
void sdram_lp0_save_params(const struct sdram_params *sdram);

#endif /* __SOC_NVIDIA_TEGRA210_SDRAM_H__ */
