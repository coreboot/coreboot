/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
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

#include <amdblocks/agesawrapper.h>
#include <baseboard/variants.h>
#include <soc/smi.h>
#include <soc/southbridge.h>
#include <stdlib.h>
#include <variant/gpio.h>

static const GPIO_CONTROL agesa_board_gpios[] = {
	/* AGPIO2 PCIE/WLAN WAKE# SCI*/
	{2, Function1, FCH_GPIO_PULL_UP_ENABLE },

	/* SER TX */
	{8, Function1, FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE
						| FCH_GPIO_OUTPUT_ENABLE},
	/* SER RX */
	{9, Function1, FCH_GPIO_PULL_UP_ENABLE },

	/* AGPIO 12 */
	{12, Function2, FCH_GPIO_PULL_UP_ENABLE },

	/* TS_EN_SOC (TouchScreen enable GPIO) */
	{13, Function1, FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE
						| FCH_GPIO_OUTPUT_ENABLE},

	/* CAM_PWRON (Camera enable GPIO) */
	{14, Function1,  FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE
						| FCH_GPIO_OUTPUT_ENABLE},

	/* EC_IN_RW */
	{15, Function1, FCH_GPIO_PULL_UP_ENABLE },

	/* APU_I2C_3_SCL */
	{19, Function1, FCH_GPIO_PULL_UP_ENABLE },

	/* APU_I2C_3_SDA */
	{20, Function1, FCH_GPIO_PULL_UP_ENABLE },

	/* AGPIO22 EC_SCI */
	{22, Function1, FCH_GPIO_PULL_UP_ENABLE },

	/* APU_BT_ON# */
	{24, Function1, FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE
						| FCH_GPIO_OUTPUT_ENABLE},

	/* DEVSLP1_SSD */
	{67, Function1, FCH_GPIO_PULL_UP_ENABLE},

	/* DEVSLP1_EMMC */
	/* No Connect for now.
	 * {70, Function1, FCH_GPIO_PULL_UP_ENABLE},
	 */

	/* SPI_TPM_CS_L */
	{76, Function1, FCH_GPIO_PULL_DOWN_ENABLE | FCH_GPIO_OUTPUT_VALUE
						  | FCH_GPIO_OUTPUT_ENABLE},

	/* CAM_LED# */
	{84, Function1, FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE
						| FCH_GPIO_OUTPUT_ENABLE},

	/* TS_RST#  (TouchScreen Reset) */
	{85, Function1, FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE
						| FCH_GPIO_OUTPUT_ENABLE},

	/* WLAN_RST#_AUX */
	{119, Function2, FCH_GPIO_PULL_UP_ENABLE | FCH_GPIO_OUTPUT_VALUE
						 | FCH_GPIO_OUTPUT_ENABLE},

	/* BD_ID1 */
	{135, Function1, 0 },

	/* TPM_SERIRQ# */
	{139, Function1, FCH_GPIO_PULL_UP_ENABLE },

	/* BD_ID2 */
	{140, Function1, 0 },

	/* APU_SPI_WP */
	{142, Function1, FCH_GPIO_PULL_UP_ENABLE },

	/* BD_ID3 */
	{144, Function1, 0 },

	{-1}
};

const GPIO_CONTROL *get_gpio_table(void)
{
	return agesa_board_gpios;
}

/*
 * GPE setup table must match ACPI GPE ASL
 *  { gevent, gpe, direction, level }
 */
static const struct sci_source gpe_table[] = {

	/* EC AGPIO22/Gevent3 -> GPE 3 */
	{
		.scimap = 3,
		.gpe = 3,
		.direction = SMI_SCI_LVL_LOW,
		.level = SMI_SCI_EDG,
	},

	/* PCIE/WLAN AGPIO2/Gevent8 -> GPE8 */
	{
		.scimap = 8,
		.gpe = 8,
		.direction = SMI_SCI_LVL_LOW,
		.level = SMI_SCI_LVL,
	},

	/* EHCI USB_PME -> GPE24 */
	{
		.scimap = 24,
		.gpe = 24,
		.direction = SMI_SCI_LVL_HIGH,
		.level = SMI_SCI_LVL,
	},

	/* XHCIC0 -> GPE31 */
	{
		.scimap = 56,
		.gpe = 31,
		.direction = SMI_SCI_LVL_HIGH,
		.level = SMI_SCI_LVL,
	},
};

const struct sci_source *get_gpe_table(size_t *num)
{
	*num = ARRAY_SIZE(gpe_table);
	return gpe_table;
}

int variant_get_xhci_oc_map(uint16_t *map)
{
	*map = USB_OC2 << OC_PORT0_SHIFT;	/* USB-A Port0/4 = OC2 */
	*map |= USB_OC0 << OC_PORT1_SHIFT;	/* USB-C Port1/5 = OC0 */
	*map |= USB_OC1 << OC_PORT2_SHIFT;	/* USB-C Port2/6 = OC1 */
	*map |= USB_OC_DISABLE << OC_PORT3_SHIFT;
	return 0;
}

int variant_get_ehci_oc_map(uint16_t *map)
{
	*map = USB_OC_DISABLE_ALL;
	return 0;
}
