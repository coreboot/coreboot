/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include "../../gpio.h"

/* GPIO pins used by coreboot should be initialized in bootblock */

static const struct soc_amd_gpio gpio_set_stage_reset[] = {
	/* not LLB */
	PAD_GPI(GPIO_12, PULL_UP),
	/* not USB_OC5_L */
	PAD_GPI(GPIO_13, PULL_UP),
	/* not USB_OC4_L */
	PAD_GPI(GPIO_14, PULL_UP),
	/* not USB_OC1_L */
	PAD_GPI(GPIO_17, PULL_UP),
	/* not USB_OC2_L */
	PAD_GPI(GPIO_18, PULL_UP),
	/* SDIO eMMC power control */
	PAD_NF(GPIO_22, EMMC_PWR_CTRL, PULL_NONE),
	/* PCIe SSD power enable */
	PAD_GPO(GPIO_23, HIGH),
	/* PCIe Reset to DP0, DP1, J2105, TP, FP */
	PAD_NF(GPIO_27, PCIE_RST1_L, PULL_NONE),
	/* eSPI CS# */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),
	/* FANOUT0 */
	PAD_NF(GPIO_85, FANOUT0, PULL_NONE),
	/* PC beep to codec */
	PAD_NF(GPIO_91, SPKR, PULL_NONE),
};

void mainboard_program_early_gpios(void)
{
	gpio_configure_pads(gpio_set_stage_reset, ARRAY_SIZE(gpio_set_stage_reset));
}
