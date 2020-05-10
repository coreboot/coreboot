/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_NVIDIA_TEGRA124_POWER_H__
#define __SOC_NVIDIA_TEGRA124_POWER_H__

// This function does not enable the external power to the rail, it enables
// the rail itself internal to the SOC.
void power_enable_and_ungate_cpu(void);

// power_reset_status returns one of the following possible sources for the
// most recent reset.
enum {
	POWER_RESET_POR = 0,
	POWER_RESET_WATCHDOG = 1,
	POWER_RESET_SENSOR = 2,
	POWER_RESET_SW_MAIN = 3,
	POWER_RESET_LP0 = 4
};
int power_reset_status(void);

void ram_repair(void);

#endif	/* __SOC_NVIDIA_TEGRA124_POWER_H__ */
