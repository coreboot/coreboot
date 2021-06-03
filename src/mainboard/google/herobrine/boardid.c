/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <ec/google/chromeec/ec.h>
#include "board.h"
#include <commonlib/bsd/cb_err.h>
#include <console/console.h>
#include <gpio.h>

uint32_t board_id(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;
	const gpio_t pins[] = {[2] = GPIO(50), [1] = GPIO(49), [0] = GPIO(48)};

	if (id == UNDEFINED_STRAPPING_ID)
		id = gpio_base3_value(pins, ARRAY_SIZE(pins));

	if ((id == QCOM_SC7280_SKU1) || (id == QCOM_SC7280_SKU2) ||
						(id == QCOM_SC7280_SKU3))
		printk(BIOS_INFO, "BoardID :%d - "
				"Machine model: "
				"Qualcomm Technologies, Inc. "
				"sc7280 IDP SKU%d platform\n", id, (id+1));
	else
		printk(BIOS_ERR, "Invalid BoardId : %d\n", id);

	return id;
}

uint32_t ram_code(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;

	return id;
}

uint32_t sku_id(void)
{
	return google_chromeec_get_board_sku();
}
