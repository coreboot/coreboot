/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __SOC_NVIDIA_TEGRA_PINGROUP_H__
#define __SOC_NVIDIA_TEGRA_PINGROUP_H__

#include <stdint.h>

void pingroup_set_config(int group_index, uint32_t config);
uint32_t pingroup_get_config(int group_index);

enum {
	PINGROUP_HSM = 1 << 2,
	PINGROUP_SCHMT = 1 << 3,
	PINGROUP_LPMD_SHIFT = 4,
	PINGROUP_LPMD_MASK = 3 << 4,
	PINGROUP_DRVDN_SHIFT = 12,
	PINGROUP_DRVDN_MASK = 0x7f << 12,
	PINGROUP_DRVUP_SHIFT = 20,
	PINGROUP_DRVUP_MASK = 0x7f << 20,
	PINGROUP_SLWR_SHIFT = 28,
	PINGROUP_SLWR_MASK = 0x3 << 28,
	PINGROUP_SLWF_SHIFT = 30,
	PINGROUP_SLWF_MASK = 0x3 << 30
};

#endif	/* __SOC_NVIDIA_TEGRA_PINGROUP_H__ */
