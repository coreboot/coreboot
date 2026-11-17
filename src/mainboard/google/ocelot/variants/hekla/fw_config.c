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

static const struct pad_config ish_enable_pads[] = {
	/* GPP_B05:     EC_ISH_INT_ODL (ISH_GP1) */
	PAD_CFG_NF(GPP_B05, NONE, DEEP, NF4),
	/* GPP_B18:     ISH_I2C2_SDA */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_B18, NONE, DEEP, NF1),
	/* GPP_B19:     ISH_I2C2_SCL */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_B19, NONE, DEEP, NF1),
};

static const struct pad_config ish_disable_pads[] = {
	/* GPP_B05:     EC_ISH_INT_ODL (ISH_GP1) ==> NC*/
	PAD_NC(GPP_B05, NONE),
	/* GPP_B18:     ISH_I2C2_SDA ==> NC*/
	PAD_NC(GPP_B18, NONE),
	/* GPP_B19:     ISH_I2C2_SCL ==> NC*/
	PAD_NC(GPP_B19, NONE),
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}

	if (fw_config_probe(FW_CONFIG(SENSOR_HUB, ISH_PRESENT))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, ish_enable_pads);
	} else {
		GPIO_PADBASED_OVERRIDE(padbased_table, ish_disable_pads);
	}
}
