/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI_SAR));
}

static const struct pad_config ish_disable_pads[] = {
	/* B3  : ISH_IMU_INT_L ==>NC */
	PAD_NC(GPP_B3, NONE),
	/* B4  : ISH_ACC_INT_L ==>NC */
	PAD_NC(GPP_B4, NONE),
	/* B5  : GPP_B5 ==> NC */
	PAD_NC(GPP_B5, NONE),
	/* B6  : GPP_B6 ==> NC */
	PAD_NC(GPP_B6, NONE),
	/* D1  : ISH_GP1 ==> NC */
	PAD_NC(GPP_D1, NONE),
	/* D13  : UART0_ISH_RXD ==> NC */
	PAD_NC(GPP_D13, NONE),
	/* D14  : UART0_ISH_TXD ==> NC */
	PAD_NC(GPP_D14, NONE),
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (fw_config_probe(FW_CONFIG(TABLET_MODE, TABLET_MODE_ABSENT))) {
		printk(BIOS_INFO, "Disable ISH GPIO pins.\n");
		gpio_padbased_override(padbased_table, ish_disable_pads, ARRAY_SIZE(ish_disable_pads));
	}
}

void variant_configure_pads(void)
{
	const struct pad_config *base_pads;
	struct pad_config *padbased_table;
	size_t base_num;

	padbased_table = new_padbased_table();
	base_pads = variant_gpio_table(&base_num);
	gpio_padbased_override(padbased_table, base_pads, base_num);
	fw_config_gpio_padbased_override(padbased_table);
	gpio_configure_pads_with_padbased(padbased_table);
	free(padbased_table);
}
