/*
 * Power setup code for EXYNOS5
 *
 * Copyright (C) 2012 Samsung Electronics
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/arch/power.h>
#include <asm/arch/sysreg.h>
#include <asm/arch-exynos/spl.h>
#include <i2c.h>
#include <max77686.h>

static void ps_hold_setup(void)
{
	struct exynos5_power *power =
		(struct exynos5_power *)samsung_get_base_power();

	/* Set PS-Hold high */
	setbits_le32(&power->ps_hold_ctrl, POWER_PS_HOLD_CONTROL_DATA_HIGH);
}

void power_reset(void)
{
	struct exynos5_power *power =
		(struct exynos5_power *)samsung_get_base_power();

	/* Clear inform1 so there's no change we think we've got a wake reset */
	power->inform1 = 0;

	setbits_le32(&power->sw_reset, 1);
}

/* This function never returns */
void power_shutdown(void)
{
	struct exynos5_power *power =
		(struct exynos5_power *)samsung_get_base_power();

	clrbits_le32(&power->ps_hold_ctrl, POWER_PS_HOLD_CONTROL_DATA_HIGH);

	hang();
}

void power_enable_dp_phy(void)
{
	struct exynos5_power *power =
		(struct exynos5_power *)samsung_get_base_power();

	setbits_le32(&power->dptx_phy_control, DPTX_PHY_ENABLE);
}

void power_enable_usb_phy(void)
{
	struct exynos5_sysreg *sysreg =
		(struct exynos5_sysreg *)samsung_get_base_sysreg();
	struct exynos5_power *power =
		(struct exynos5_power *)samsung_get_base_power();
	unsigned int phy_cfg;

	/* Setting USB20PHY_CONFIG register to USB 2.0 HOST link */
	phy_cfg = readl(&sysreg->usb20_phy_cfg);
	if (phy_cfg & USB20_PHY_CFG_EN) {
		debug("USB 2.0 HOST link already selected\n");
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
		(struct exynos5_power *)samsung_get_base_power();

	/* Disabling USBHost_PHY */
	clrbits_le32(&power->usb_host_phy_ctrl, POWER_USB_HOST_PHY_CTRL_EN);
}

void power_enable_hw_thermal_trip(void)
{
	struct exynos5_power *power =
		(struct exynos5_power *)samsung_get_base_power();

	/* Enable HW thermal trip */
	setbits_le32(&power->ps_hold_ctrl, POWER_ENABLE_HW_TRIP);
}

uint32_t power_read_reset_status(void)
{
	struct exynos5_power *power =
		(struct exynos5_power *)samsung_get_base_power();

	return power->inform1;
}

void power_exit_wakeup(void)
{
	struct exynos5_power *power =
		(struct exynos5_power *)samsung_get_base_power();
	typedef void (*resume_func)(void);

	((resume_func)power->inform0)();
}

/**
 * Initialize the pmic voltages to power up the system
 * This also calls i2c_init so that we can program the pmic
 *
 * REG_ENABLE = 0, needed to set the buck/ldo enable bit ON
 *
 * @return	Return 0 if ok, else -1
 */
int power_init(void)
{
	int error = 0;

#ifdef CONFIG_SPL_BUILD
	struct spl_machine_param *param = spl_get_machine_params();

	/* Set the i2c register address base so i2c works before FDT */
	i2c_set_early_reg(param->i2c_base);
#endif

	ps_hold_setup();

	/* init the i2c so that we can program pmic chip */
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);

	/*
	 * We're using CR1616 coin cell battery that is non-rechargeable
	 * battery. But, BBCHOSTEN bit of the BBAT Charger Register in
	 * MAX77686 is enabled by default for charging coin cell.
	 *
	 * Also, we cannot meet the coin cell reverse current spec. in UL
	 * standard if BBCHOSTEN bit is enabled.
	 *
	 * Disable Coin BATT Charging
	 */
	error = max77686_disable_backup_batt();

	error |= max77686_volsetting(PMIC_BUCK2, CONFIG_VDD_ARM_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(PMIC_BUCK3, CONFIG_VDD_INT_UV,
						REG_ENABLE, MAX77686_UV);
	error |= max77686_volsetting(PMIC_BUCK1, CONFIG_VDD_MIF_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(PMIC_BUCK4, CONFIG_VDD_G3D_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(PMIC_LDO2, CONFIG_VDD_LDO2_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(PMIC_LDO3, CONFIG_VDD_LDO3_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(PMIC_LDO5, CONFIG_VDD_LDO5_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(PMIC_LDO10, CONFIG_VDD_LDO10_MV,
						REG_ENABLE, MAX77686_MV);
	if (error != 0)
		debug("power init failed\n");

	return error;
}

void power_enable_xclkout(void)
{
	struct exynos5_power *power =
		(struct exynos5_power *)samsung_get_base_power();

	/* use xxti for xclk out */
	clrsetbits_le32(&power->pmu_debug, PMU_DEBUG_CLKOUT_SEL_MASK,
				PMU_DEBUG_XXTI);
}
