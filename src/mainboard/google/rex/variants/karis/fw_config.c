/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <bootstate.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>

#define GPIO_PADBASED_OVERRIDE(b, a) gpio_padbased_override(b, a, ARRAY_SIZE(a))

static const struct pad_config fp_disable_pads[] = {
	/* GPP_B11 : [] ==> EN_FP_PWR */
	PAD_NC(GPP_B11, NONE),
	/* GPP_C22 : [] ==> SOC_FP_BOOT0 */
	PAD_NC(GPP_C22, NONE),
	/* GPP_C23 : [] ==> FP_RST_ODL */
	PAD_NC(GPP_C23, NONE),
	/* GPP_E10 : [] ==> SOC_FPMCU_INT_L */
	PAD_NC(GPP_E10, NONE),
	/* GPP_F11 : GSP1_SOC_CLK_R */
	PAD_NC(GPP_F11, NONE),
	/* GPP_F12 : GSPI1_SOC_DO_FPMCU_DI_R */
	PAD_NC(GPP_F12, NONE),
	/* GPP_F13 : GSPI1_SOC_DI_FPMCU_DO_LS_R */
	PAD_NC(GPP_F13, NONE),
	/* GPP_F17 : [] ==> GSPI1_SOC_CS_L */
	PAD_NC(GPP_F17, NONE),
};

static const struct pad_config stylus_disable_pads[] = {
	/* GPP_E04 : SOC_PEN_DETECT */
	PAD_NC(GPP_E04, NONE),
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (fw_config_probe(FW_CONFIG(STYLUS, STYLUS_ABSENT))) {
		printk(BIOS_INFO, "Configure GPIOs for no stylus.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, stylus_disable_pads);
	}
	if (fw_config_probe(FW_CONFIG(FP_MCU, FP_ABSENT))) {
		printk(BIOS_INFO, "Configure GPIOs for no FP module.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, fp_disable_pads);
	}
}
