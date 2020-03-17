/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
