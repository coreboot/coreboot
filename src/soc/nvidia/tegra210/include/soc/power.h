/* SPDX-License-Identifier: GPL-2.0-only */

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
