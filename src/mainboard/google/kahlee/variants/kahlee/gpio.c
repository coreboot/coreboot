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

/*
 * As a rule of thumb, GPIO pins used by coreboot should be initialized at
 * bootblock while GPIO pins used only by the OS should be initialized at
 * ramstage.
 */
const struct soc_amd_stoneyridge_gpio gpio_set_stage_reset[] = {
	/* AGPIO2, to become event generator */
	{ GPIO_2, Function1, FCH_GPIO_PULL_UP_ENABLE | INPUT },

	/* SER_TX */
	{ GPIO_8, Function1, FCH_GPIO_PULL_UP_ENABLE | OUTPUT_H },

	/* SER RX */
	{ GPIO_9, Function1, FCH_GPIO_PULL_UP_ENABLE | INPUT },

	/* EC_IN_RW */
	{GPIO_15, Function1, FCH_GPIO_PULL_UP_ENABLE | INPUT },

	/* APU_I2C_3_SCL */
	{GPIO_19, Function1, FCH_GPIO_PULL_UP_ENABLE | INPUT },

	/* APU_I2C_3_SDA */
	{GPIO_20, Function1, FCH_GPIO_PULL_UP_ENABLE | INPUT },

	/* AGPIO22 EC_SCI */
	{GPIO_22, Function1, FCH_GPIO_PULL_UP_ENABLE | INPUT },

	/* SPI_TPM_CS_L */
	{GPIO_76, Function1, FCH_GPIO_PULL_DOWN_ENABLE | OUTPUT_H },

	/* BD_ID1 */
	{GPIO_135, Function1, INPUT },

	/* GPIO_136 - UART_FCH_RX_DEBUG_RX */
	{GPIO_136, Function0, INPUT },

	/* GPIO_138 - UART_FCH_TX_DEBUG_RX */
	{GPIO_138, Function0, INPUT },

	/* TPM_SERIRQ# */
	{GPIO_139, Function1, FCH_GPIO_PULL_UP_ENABLE | INPUT },

	/* BD_ID2 */
	{GPIO_140, Function1, INPUT },

	/* APU_SPI_WP */
	{GPIO_142, Function1, FCH_GPIO_PULL_UP_ENABLE | INPUT },

	/* BD_ID3 */
	{GPIO_144, Function1, INPUT }
};

const struct soc_amd_stoneyridge_gpio gpio_set_stage_ram[] = {
	/* AGPIO 12 */
	{ GPIO_12, Function2, FCH_GPIO_PULL_UP_ENABLE | INPUT },

	/* TS_EN_SOC (TouchScreen enable GPIO) */
	{GPIO_13, Function1, FCH_GPIO_PULL_UP_ENABLE | OUTPUT_H},

	/* CAM_PWRON (Camera enable GPIO) */
	{GPIO_14, Function1, FCH_GPIO_PULL_UP_ENABLE | OUTPUT_H },
	/* APU_BT_ON# */
	{GPIO_24, Function1, FCH_GPIO_PULL_UP_ENABLE | OUTPUT_H },

	/* DEVSLP1_SSD */
	{67, Function1, FCH_GPIO_PULL_UP_ENABLE | INPUT },

	/* DEVSLP1_EMMC */
	/* No Connect for now.
	 * {GPIO_70, Function1, FCH_GPIO_PULL_UP_ENABLE | INPUT},
	 */

	/* CAM_LED# */
	{GPIO_84, Function1, FCH_GPIO_PULL_UP_ENABLE | OUTPUT_H },

	/* TS_RST#  (TouchScreen Reset) */
	{GPIO_85, Function1, FCH_GPIO_PULL_UP_ENABLE | OUTPUT_H },

	/* WLAN_RST#_AUX */
	{GPIO_119, Function2, FCH_GPIO_PULL_UP_ENABLE | OUTPUT_H },
};

const struct soc_amd_stoneyridge_gpio *variant_early_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(gpio_set_stage_reset);
	return gpio_set_stage_reset;
}

const struct soc_amd_stoneyridge_gpio *variant_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(gpio_set_stage_ram);
	return gpio_set_stage_ram;
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
