/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include "gpio.h"

/* eMMC controller driving either an SD card or eMMC device. */
static const struct soc_amd_gpio emmc_gpios[] = {
	PAD_NF(GPIO_21,  EMMC_CMD,	PULL_UP),
	PAD_NF(GPIO_22,  EMMC_PWR_CTRL,	PULL_UP),
	PAD_NF(GPIO_68,  EMMC_CD,	PULL_UP),
	PAD_NF(GPIO_70,  EMMC_CLK,	PULL_NONE),
	PAD_NF(GPIO_104, EMMC_DATA0,	PULL_UP),
	PAD_NF(GPIO_105, EMMC_DATA1,	PULL_UP),
	PAD_NF(GPIO_106, EMMC_DATA2,	PULL_UP),
	PAD_NF(GPIO_107, EMMC_DATA3,	PULL_NONE),
	PAD_NF(GPIO_74,  EMMC_DATA4,	PULL_UP),
	PAD_NF(GPIO_75,  EMMC_DATA6,	PULL_UP),
	PAD_NF(GPIO_87,  EMMC_DATA7,	PULL_UP),
	PAD_NF(GPIO_88,  EMMC_DATA5,	PULL_UP),
	PAD_NF(GPIO_109, EMMC_DS,	PULL_UP),
};

/* Don't call this if the board uses the LPC bus. */
void mainboard_program_emmc_gpios(void)
{
	gpio_configure_pads(emmc_gpios, ARRAY_SIZE(emmc_gpios));
}
