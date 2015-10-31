/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Samsung Electronics
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

/* Power setup code for EXYNOS5 */

#include <arch/io.h>
#include <console/console.h>
#include <halt.h>
#include <soc/dmc.h>
#include <soc/power.h>
#include <soc/setup.h>

/* Set the PS-Hold drive value */
static void ps_hold_setup(void)
{
	/* Set PS-Hold high */
	setbits_le32(&exynos_power->ps_hold_ctrl,
		     POWER_PS_HOLD_CONTROL_DATA_HIGH);
}

void power_reset(void)
{
	/* Clear inform1 so there's no change we think we've got a wake reset */
	exynos_power->inform1 = 0;

	setbits_le32(&exynos_power->sw_reset, 1);
}

/* This function never returns */
void power_shutdown(void)
{
	clrbits_le32(&exynos_power->ps_hold_ctrl,
		     POWER_PS_HOLD_CONTROL_DATA_HIGH);

	halt();
}

void power_enable_dp_phy(void)
{
	setbits_le32(&exynos_power->dptx_phy_control, EXYNOS_DP_PHY_ENABLE);
}

void power_enable_hw_thermal_trip(void)
{
	/* Enable HW thermal trip */
	setbits_le32(&exynos_power->ps_hold_ctrl, POWER_ENABLE_HW_TRIP);
}

uint32_t power_read_reset_status(void)
{
	return exynos_power->inform1;
}

void power_exit_wakeup(void)
{
	typedef void (*resume_func)(void);

	((resume_func)exynos_power->inform0)();
}

int power_init(void)
{
	ps_hold_setup();
	return 0;
}

void power_enable_xclkout(void)
{
	/* use xxti for xclk out */
	clrsetbits_le32(&exynos_power->pmu_debug, PMU_DEBUG_CLKOUT_SEL_MASK,
			PMU_DEBUG_XXTI);
}

void power_release_uart_retention(void)
{
	write32(&exynos_power->padret_uart_opt, 1 << 28);
}
