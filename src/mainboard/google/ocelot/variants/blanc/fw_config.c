/* SPDX-License-Identifier: GPL-2.0-only */

#include <fw_config.h>
#include <sar.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <gpio.h>

#define GPIO_PADBASED_OVERRIDE(t, o) gpio_padbased_override(t, o, ARRAY_SIZE(o))

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(AP_OEM_3BIT_FIELD0));
}

static const struct pad_config touchpad_lpss_i2c_enable_pads[] = {
	/* GPP_E12:     THC_I2C4_SCL_TCH_PAD */
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF8),
	/* GPP_E13:     THC_I2C4_SDA_TCH_PAD */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF8),
	/* GPP_E18:     TCH_PAD_INT_N */
	PAD_CFG_GPI_APIC(GPP_E18, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_VGPIO3_THC0: THC0_WOT */
	PAD_NC(GPP_VGPIO3_THC0, NONE),
};

static const struct pad_config touchpad_thc_i2c_enable_pads[] = {
	/* GPP_E12:     NF1: THC_I2C4_SCL */
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF1),
	/* GPP_E13:     NF1: THC_I2C4_SDA */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF1),
	/* GPP_E18:     TCH_PAD_INT_N */
	/* NOTE: this INT NF is used in THC-I2C mode */
	PAD_CFG_NF(GPP_E18, NONE, DEEP, NF3),
	/* GPP_VGPIO3_THC0: THC0_WOT */
	PAD_NC(GPP_VGPIO3_THC0, NONE),
};

static const struct pad_config touchpad_i2c_disable_pads[] = {
	/* GPP_E12:     THC_I2C1_SCL_TCH_PAD */
	PAD_NC(GPP_E12, NONE),
	/* GPP_E13:     THC_I2C1_SDA_TCH_PAD */
	PAD_NC(GPP_E13, NONE),
	/* GPP_E18:     TCH_PAD_INT_N */
	/* NOTE: require rework to switch from GPP_A13 to GPP_F18 */
	PAD_NC(GPP_E18, NONE),
	/* GPP_VGPIO3_THC0: THC0_WOT */
	PAD_NC(GPP_VGPIO3_THC0, NONE),
};

static const struct pad_config thc0_enable_wake[] = {
	/* GPP_VGPIO3_THC0: THC0_WOT */
	PAD_CFG_GPI_APIC_DRIVER(GPP_VGPIO3_THC0, NONE, PLTRST, LEVEL, NONE),
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	const struct soc_intel_pantherlake_config *config = config_of_soc();

	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}

	if (fw_config_probe(FW_CONFIG(TRACKPAD, TRACKPAD_ELAN))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, touchpad_lpss_i2c_enable_pads);
	} else if(fw_config_probe(FW_CONFIG(TRACKPAD, TRACKPAD_FOCAL))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, touchpad_thc_i2c_enable_pads);
		if (config->thc_wake_on_touch[0])
			GPIO_PADBASED_OVERRIDE(padbased_table, thc0_enable_wake);
	} else {
		GPIO_PADBASED_OVERRIDE(padbased_table, touchpad_i2c_disable_pads);
	}
}
