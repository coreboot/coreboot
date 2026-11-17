/* SPDX-License-Identifier: GPL-2.0-only */

#include <fw_config.h>
#include <sar.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <delay.h>
#include <gpio.h>
#include <inttypes.h>

/* t: base table; o: override table */
#define GPIO_PADBASED_OVERRIDE(t, o) gpio_padbased_override(t, o, ARRAY_SIZE(o))

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(AP_OEM_3BIT_FIELD0));
}

static const struct pad_config ish_disable_pads[] = {
	/* GPP_B05:     C_EC_ISH_ALRT ==> NC */
	PAD_NC(GPP_B05, NONE),
	/* GPP_B18:     ISH_I2C2_SDA_SNSR_HDR ==> NC*/
	PAD_NC(GPP_B18, NONE),
	/* GPP_B19:     ISH_I2C2_SCL_SNSR_HDR ==> NC */
	PAD_NC(GPP_B19, NONE),
};

static const struct pad_config ish_enable_pads[] = {
	/* GPP_B05:     C_EC_ISH_ALRT */
	PAD_CFG_NF(GPP_B05, NONE, DEEP, NF4),
	/* GPP_B18:     ISH_I2C2_SDA_SNSR_HDR */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_B18, NONE, DEEP, NF1),
	/* GPP_B19:     ISH_I2C2_SCL_SNSR_HDR */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_B19, NONE, DEEP, NF1),
	/* GPP_D05:     UART_ISH_RX_DBG_TX ==> NC */
	PAD_NC(GPP_D05, NONE),
	/* GPP_D06:     UART_ISH_TX_DBG_RX ==> NC */
	PAD_NC(GPP_D06, NONE),
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (fw_config_probe(FW_CONFIG(SENSOR_HUB, ISH_PRESENT))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, ish_enable_pads);
	} else {
		GPIO_PADBASED_OVERRIDE(padbased_table, ish_disable_pads);
	}
}
