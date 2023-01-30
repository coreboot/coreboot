/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include "../../gpio.h"

/*
 * As a rule of thumb, GPIO pins used by coreboot should be initialized at
 * bootblock while GPIO pins used only by the OS should be initialized at
 * ramstage.
 */
static const struct soc_amd_gpio gpio_set_stage_ram[] = {
	/* GPIO_2 - WLAN_PCIE_WAKE_3V3_ODL, SCI */
	PAD_NF_SCI(GPIO_2, WAKE_L, PULL_UP, EDGE_LOW),
	/* SSD DEVSLP */
	PAD_NF(GPIO_5, DEVSLP0, PULL_NONE),
	/* Defeature SATA Express DEVSLP, as some boards are reworked
	 * to tie this to GPIO23 to control power */
	PAD_GPI(GPIO_6, PULL_UP),
	/* I2S SDIN */
	PAD_NF(GPIO_7, ACP_I2S_SDIN, PULL_NONE),
	/* I2S LRCLK */
	PAD_NF(GPIO_8, ACP_I2S_LRCLK, PULL_NONE),
	/* Blink */
	PAD_NF(GPIO_11, BLINK, PULL_NONE),
	/* APU_ALS_INT# */
	PAD_SCI(GPIO_24, PULL_UP, EDGE_LOW),
	/* Finger print CS# */
	PAD_GPO(GPIO_31, HIGH),
	/* NFC IRQ */
	PAD_INT(GPIO_69, PULL_UP, EDGE_LOW, STATUS),
	/* Rear camera power enable */
	PAD_GPO(GPIO_89, HIGH),
};

void mainboard_program_gpios(void)
{
	gpio_configure_pads(gpio_set_stage_ram, ARRAY_SIZE(gpio_set_stage_ram));
}
