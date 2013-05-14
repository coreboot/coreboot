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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* Power setup code for EXYNOS5 */

#include <console/console.h>
#include <arch/io.h>
#include <arch/hlt.h>
#include "cpu.h"
#include "power.h"
#include "sysreg.h"

static void ps_hold_setup(void)
{
	struct exynos5_power *power =
		samsung_get_base_power();

	/* Set PS-Hold high */
	setbits_le32(&power->ps_hold_ctrl, POWER_PS_HOLD_CONTROL_DATA_HIGH);
}

void power_reset(void)
{
	struct exynos5_power *power =
		samsung_get_base_power();

	/* Clear inform1 so there's no change we think we've got a wake reset */
	power->inform1 = 0;

	setbits_le32(&power->sw_reset, 1);
}

/* This function never returns */
void power_shutdown(void)
{
	struct exynos5_power *power =
		samsung_get_base_power();

	clrbits_le32(&power->ps_hold_ctrl, POWER_PS_HOLD_CONTROL_DATA_HIGH);

	hlt();
}

void power_enable_dp_phy(void)
{
	struct exynos5_power *power =
		samsung_get_base_power();

	setbits_le32(&power->dptx_phy_control, DPTX_PHY_ENABLE);
}

void power_enable_usb_phy(void)
{
	struct exynos5_sysreg *sysreg =
		samsung_get_base_sysreg();
	struct exynos5_power *power =
		samsung_get_base_power();
	unsigned int phy_cfg;

	/* Setting USB20PHY_CONFIG register to USB 2.0 HOST link */
	phy_cfg = readl(&sysreg->usb20_phy_cfg);
	if (phy_cfg & USB20_PHY_CFG_EN) {
		printk(BIOS_DEBUG, "USB 2.0 HOST link already selected\n");
	} else {
		phy_cfg |= USB20_PHY_CFG_EN;
		writel(phy_cfg, &sysreg->usb20_phy_cfg);
	}

	/* Enabling USBHOST_PHY */
	setbits_le32(&power->usb_host_phy_ctrl, POWER_USB_HOST_PHY_CTRL_EN);
}

void power_disable_usb_phy(void)
{
	struct exynos5_power *power =
		samsung_get_base_power();

	/* Disabling USBHost_PHY */
	clrbits_le32(&power->usb_host_phy_ctrl, POWER_USB_HOST_PHY_CTRL_EN);
}

void power_enable_hw_thermal_trip(void)
{
	struct exynos5_power *power =
		samsung_get_base_power();

	/* Enable HW thermal trip */
	setbits_le32(&power->ps_hold_ctrl, POWER_ENABLE_HW_TRIP);
}

uint32_t power_read_reset_status(void)
{
	struct exynos5_power *power =
		samsung_get_base_power();

	return power->inform1;
}

void power_exit_wakeup(void)
{
	struct exynos5_power *power =
		samsung_get_base_power();
	typedef void (*resume_func)(void);

	((resume_func)power->inform0)();
}

int power_init(void)
{
	ps_hold_setup();
	return 0;
}

void power_enable_xclkout(void)
{
	struct exynos5_power *power =
		samsung_get_base_power();

	/* use xxti for xclk out */
	clrsetbits_le32(&power->pmu_debug, PMU_DEBUG_CLKOUT_SEL_MASK,
				PMU_DEBUG_XXTI);
}
