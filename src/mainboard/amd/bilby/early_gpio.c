/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include "gpio.h"

/* GPIO pins used by coreboot should be initialized in bootblock */

static const struct soc_amd_gpio gpio_set_stage_reset[] = {
	/* assert PCIe reset */
	PAD_GPO(GPIO_6, HIGH),
	/* not LLB */
	PAD_GPI(GPIO_12, PULL_UP),
	/* not USB_OC1_L */
	PAD_GPI(GPIO_17, PULL_UP),
	/* not USB_OC2_L */
	PAD_GPI(GPIO_18, PULL_UP),
	/* SDIO eMMC power control */
	PAD_NF(GPIO_22, EMMC_PWR_CTRL, PULL_NONE),
	/* PCIe Reset to DP0, DP1, J2105, TP, FP */
	PAD_GPO(GPIO_27, HIGH),
	/* eSPI CS# */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),
	/* GPP_10G_SELECT => High=10G, Low=x2 NVME (work with AGPIO89) */
	PAD_GPO(GPIO_42, LOW),
	/* FANOUT0 */
	PAD_NF(GPIO_85, FANOUT0, PULL_NONE),
	/* APU_COMBO_GPP_SW => High=SATA, Low=x2 NVME (work with EGPIO42) */
	PAD_GPO(GPIO_89, LOW),
	/* PC beep to codec */
	PAD_NF(GPIO_91, SPKR, PULL_NONE),
};

void mainboard_program_early_gpios(void)
{
	gpio_configure_pads(gpio_set_stage_reset, ARRAY_SIZE(gpio_set_stage_reset));
}
