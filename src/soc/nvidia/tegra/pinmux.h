/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __SOC_NVIDIA_TEGRA_PINMUX_H__
#define __SOC_NVIDIA_TEGRA_PINMUX_H__

#include <stdint.h>

void pinmux_set_config(int pin_index, uint32_t config);
uint32_t pinmux_get_config(int pin_index);

#endif	/* __SOC_NVIDIA_TEGRA_PINMUX_H__ */
