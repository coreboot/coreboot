/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include "../../gpio.h"

/*
 * As a rule of thumb, GPIO pins used by coreboot should be initialized at
 * bootblock while GPIO pins used only by the OS should be initialized at
 * ramstage.
 */
static const struct soc_amd_gpio gpio_set_stage_ram[] = {
	/* EC SCI# */
	PAD_SCI(GPIO_6, PULL_UP, EDGE_LOW),
	/* I2S SDIN */
	PAD_NF(GPIO_7, ACP_I2S_SDIN, PULL_NONE),
	/* I2S LRCLK */
	PAD_NF(GPIO_8, ACP_I2S_LRCLK, PULL_NONE),
	/* not Blink */
	PAD_GPI(GPIO_11, PULL_UP),
	/* APU_ALS_INT# */
	PAD_SCI(GPIO_24, PULL_UP, EDGE_LOW),
	/* SD card detect */
	PAD_GPI(GPIO_31, PULL_UP),
	/* NFC IRQ */
	PAD_INT(GPIO_69, PULL_UP, EDGE_LOW, STATUS),
	/* NFC wake output# */
	PAD_GPO(GPIO_89, HIGH),
};

void mainboard_program_gpios(void)
{
	gpio_configure_pads(gpio_set_stage_ram, ARRAY_SIZE(gpio_set_stage_ram));
}
