/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <baseboard/variants.h>
#include <chip.h>
#include <gpio.h>
#include <device/device.h>
#include <smbios.h>
#include <string.h>
#include <variant/sku.h>

uint32_t variant_board_sku(void)
{
	static uint32_t sku_id = SKU_UNKNOWN;

	if (sku_id != SKU_UNKNOWN)
		return sku_id;

	/*
	*  Nautilus uses GPP_B20 to determine SKU
	*  0 - Wifi SKU
	*  1 - LTE SKU
	*/
	gpio_input_pulldown(GPP_B20);
	if (!gpio_get(GPP_B20))
		sku_id = SKU_0_NAUTILUS;
	else
		sku_id = SKU_1_NAUTILUS_LTE;

	return sku_id;
}

const char *smbios_mainboard_sku(void)
{
	static char sku_str[5]; /* sku{0-1} */

	snprintf(sku_str, sizeof(sku_str), "sku%u", variant_board_sku());

	return sku_str;
}

/* Override dev tree settings per board */
void variant_devtree_update(void)
{
	uint32_t sku_id = variant_board_sku();
	struct device *root = SA_DEV_ROOT;
	config_t *cfg = root->chip_info;

	switch (sku_id) {
	case SKU_0_NAUTILUS:
		/* Disable LTE module */
		cfg->usb3_ports[3].enable = 0;
		break;

	case SKU_1_NAUTILUS_LTE:
		/* LTE board has different layout with Wifi sku, it need
		   new USB2 port strength settings */

		/* Configure USB2 port 0 - USB2_PORT_TYPE_C(OC1) */
		cfg->usb2_ports[0].enable = 1;
		cfg->usb2_ports[0].ocpin = OC1;
		cfg->usb2_ports[0].tx_bias = USB2_BIAS_0MV;
		cfg->usb2_ports[0].tx_emp_enable = USB2_PRE_EMP_ON;
		cfg->usb2_ports[0].pre_emp_bias = USB2_BIAS_56MV;
		cfg->usb2_ports[0].pre_emp_bit = USB2_HALF_BIT_PRE_EMP;

		/* Configure USB2 port 1 - USB2_PORT_LONG(OC2) */
		cfg->usb2_ports[1].enable = 1;
		cfg->usb2_ports[1].ocpin = OC2;
		cfg->usb2_ports[1].tx_bias = USB2_BIAS_39MV;
		cfg->usb2_ports[1].tx_emp_enable = USB2_PRE_EMP_ON;
		cfg->usb2_ports[1].pre_emp_bias = USB2_BIAS_56MV;
		cfg->usb2_ports[1].pre_emp_bit = USB2_HALF_BIT_PRE_EMP;

		/* Configure USB2 port 4 - USB2_PORT_TYPE_C(OC0) */
		cfg->usb2_ports[4].enable = 1;
		cfg->usb2_ports[4].ocpin = OC0;
		cfg->usb2_ports[4].tx_bias = USB2_BIAS_0MV;
		cfg->usb2_ports[4].tx_emp_enable = USB2_PRE_EMP_ON;
		cfg->usb2_ports[4].pre_emp_bias = USB2_BIAS_56MV;
		cfg->usb2_ports[4].pre_emp_bit = USB2_HALF_BIT_PRE_EMP;
		break;

	default:
		break;
	}
}
