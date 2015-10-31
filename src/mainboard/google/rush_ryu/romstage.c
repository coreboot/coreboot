/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <delay.h>
#include <soc/addressmap.h>
#include <device/i2c.h>
#include <soc/clock.h>
#include <soc/funitcfg.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/padconfig.h>
#include <soc/romstage.h>

#include "gpio.h"
#include "pmic.h"

static const struct pad_config padcfgs[] = {
	/* AP_SYS_RESET_L */
	PAD_CFG_GPIO_OUT1(GPIO_PI5, PINMUX_PULL_UP),
	/* WP_L */
	PAD_CFG_GPIO_INPUT(KB_ROW1, PINMUX_PULL_NONE),
	/* MODEM_RESET */
	PAD_CFG_GPIO_OUT0(KB_ROW11, PINMUX_PULL_DOWN),
	/* MODEM_PWR_ON */
	PAD_CFG_GPIO_OUT0(KB_ROW12, PINMUX_PULL_DOWN),
	/* MDM_DET - expected to be pulled down by LTE modem */
	PAD_CFG_GPIO_INPUT(GPIO_PV1, PINMUX_PULL_UP),
	/* Power Button - active high / low depending on board id */
	PAD_CFG_GPIO_INPUT(KB_COL0, PINMUX_PULL_UP),
	/* BTN_AP_VOLD_L - active low */
	PAD_CFG_GPIO_INPUT(KB_COL6, PINMUX_PULL_UP),
	/* BTN_AP_VOLU_L - active low */
	PAD_CFG_GPIO_INPUT(KB_COL7, PINMUX_PULL_UP),
};

static const struct pad_config tpm_pads[] = {
	PAD_CFG_SFIO(CAM_I2C_SCL, PINMUX_INPUT_ENABLE, I2C3),
	PAD_CFG_SFIO(CAM_I2C_SDA, PINMUX_INPUT_ENABLE, I2C3),
};

static const struct pad_config ec_i2c_pads[] = {
	PAD_CFG_SFIO(GEN2_I2C_SCL, PINMUX_OPEN_DRAIN|PINMUX_INPUT_ENABLE, I2C2),
	PAD_CFG_SFIO(GEN2_I2C_SDA, PINMUX_OPEN_DRAIN|PINMUX_INPUT_ENABLE, I2C2),
};

static const struct funit_cfg funits[] = {
	/* TPM on I2C3  @ 400kHz */
	FUNIT_CFG(I2C3, PLLP, 400, tpm_pads, ARRAY_SIZE(tpm_pads)),
	/* EC on I2C2 - pulled to 3.3V @ 100kHz */
	FUNIT_CFG(I2C2, PLLP, 100, ec_i2c_pads, ARRAY_SIZE(ec_i2c_pads)),
};

static void lte_modem_init(void)
{
	int mdm_det;
	uint8_t data;

	/* A LTE modem is present if MDM_DET is pulled down by the modem */
	mdm_det = gpio_get(MDM_DET);
	if (mdm_det == 1)
		return;

	printk(BIOS_DEBUG, "Found LTE modem\n");

	/* Enable PMIC CLK32KGAUDIO to drive CLK_MDM_32K */
	pmic_read_reg(I2CPWR_BUS, TI65913_PAD2, &data);
	pmic_write_reg(I2CPWR_BUS, TI65913_PAD2,
			PAD2_GPIO_5_SEC_CLK32KGAUDIO(data), 0);
	pmic_write_reg(I2CPWR_BUS, TI65913_CLK32KGAUDIO_CTRL,
			TI65913_MODE_ACTIVE_ON, 0);

	/* FULL_CARD_POWER_OFF# (A44: MODEM_PWR_ON) and RESET#
	 * (A44: MODEM_RESET) of the LTE modem are actively low and initially
	 * pulled down by the pad config. To properly enable the LTE modem,
	 * de-assert FULL_CARD_POWER_OFF#, wait for at least 10ms, and then
	 * de-assert RESET#.
	 */
	gpio_output(MODEM_PWR_ON, 1);
	udelay(15000);
	gpio_output(MODEM_RESET, 1);
}

void romstage_mainboard_init(void)
{
	/* Bring up controller interfaces for ramstage loading. */
	soc_configure_funits(funits, ARRAY_SIZE(funits));
	soc_configure_pads(padcfgs, ARRAY_SIZE(padcfgs));

	/* TPM */
	i2c_init(I2C3_BUS);
	/* EC */
	i2c_init(I2C2_BUS);

	lte_modem_init();
}

void mainboard_configure_pmc(void)
{
}

void mainboard_enable_vdd_cpu(void)
{
	/* VDD_CPU is already enabled in bootblock. */
}
