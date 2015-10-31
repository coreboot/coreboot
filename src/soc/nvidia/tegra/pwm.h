/*
 *  Copyright 2014 Google Inc.
 *  (C) Copyright 2010
 *  NVIDIA Corporation <www.nvidia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SOC_NVIDIA_TEGRA_PWM_H
#define __SOC_NVIDIA_TEGRA_PWM_H

#include <types.h>

/* Register definitions for the Tegra pwm controller */
#define NV_PWM_CSR_ENABLE_SHIFT		31
#define NV_PWM_CSR_PULSE_WIDTH_SHIFT	16

struct pwm_reg {
	u32 csr;
	u32 rsvd[3];
};

struct pwm_controller {
	struct pwm_reg pwm[4];
};

#endif /* __SOC_NVIDIA_TEGRA_PWM_H */
