/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>

static const struct pad_config fp_disable_pads[] = {
	PAD_NC(GPP_D0, NONE),	/* D0  : ISH_GP0 ==> PCH_FP_BOOT0 */
	PAD_NC(GPP_D1, NONE),	/* D1  : ISH_GP1 ==> FP_RST_ODL */
	PAD_NC(GPP_D2, NONE),	/* D2  : ISH_GP2 ==> EN_FP_PWR */
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (fw_config_probe(FW_CONFIG(FP_MCU, FP_ABSENT))) {
		printk(BIOS_INFO, "Configure GPIOs for no FP module.\n");
		gpio_padbased_override(padbased_table, fp_disable_pads, ARRAY_SIZE(fp_disable_pads));
	}
}
