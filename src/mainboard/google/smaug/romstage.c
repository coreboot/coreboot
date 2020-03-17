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

#include <soc/addressmap.h>
#include <device/i2c_simple.h>
#include <soc/clock.h>
#include <soc/funitcfg.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/padconfig.h>
#include <soc/romstage.h>

#include "gpio.h"
#include "pmic.h"

static const struct pad_config padcfgs[] = {
	/* AP_SYS_RESET_L - active low*/
	PAD_CFG_GPIO_OUT1(SDMMC1_DAT0, PINMUX_PULL_UP),
	/* WP_L - active low */
	PAD_CFG_GPIO_INPUT(GPIO_PK2, PINMUX_PULL_NONE),
	/* BTN_AP_PWR_L - active low */
	PAD_CFG_GPIO_INPUT(BUTTON_POWER_ON, PINMUX_PULL_UP),
	/* BTN_AP_VOLD_L - active low */
	PAD_CFG_GPIO_INPUT(BUTTON_VOL_DOWN, PINMUX_PULL_UP),
	/* BTN_AP_VOLU_L - active low */
	PAD_CFG_GPIO_INPUT(SDMMC1_DAT1, PINMUX_PULL_UP),
};

void romstage_mainboard_init(void)
{
	soc_configure_pads(padcfgs, ARRAY_SIZE(padcfgs));
}

void mainboard_configure_pmc(void)
{
}

void mainboard_enable_vdd_cpu(void)
{
	/* VDD_CPU is already enabled in bootblock. */
}
