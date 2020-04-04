/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __SOC_NVIDIA_TEGRA210_SOC_ROMSTAGE_H__
#define __SOC_NVIDIA_TEGRA210_SOC_ROMSTAGE_H__

void romstage(void);
void romstage_mainboard_init(void);

void mainboard_configure_pmc(void);
void mainboard_enable_vdd_cpu(void);

#endif /* __SOC_NVIDIA_TEGRA210_SOC_ROMSTAGE_H__ */
