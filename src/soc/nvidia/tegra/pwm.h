/* SPDX-License-Identifier: GPL-2.0-or-later */

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
