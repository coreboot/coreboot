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
	PAD_GPI(GPIO_2, PULL_UP),

	/* SER_TX */
	PAD_NF(GPIO_8, SerPortTX_OUT, PULL_UP),

	/* SER RX */
	PAD_NF(GPIO_9, SerPortRX_OUT, PULL_UP),

	/* EC_IN_RW */
	PAD_GPI(GPIO_15, PULL_UP),

	/* APU_I2C_3_SCL */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_UP),

	/* APU_I2C_3_SDA */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_UP),

	/* AGPIO22 EC_SCI */
	PAD_GPI(GPIO_22, PULL_UP),

	/* SPI_TPM_CS_L */
	PAD_NF(GPIO_76, SPI_TPM_CS_L, PULL_DOWN),

	/* BD_ID1 */
	PAD_GPI(GPIO_135, PULL_NONE),

	/* GPIO_136 - UART_FCH_RX_DEBUG_RX */
	PAD_NF(GPIO_136, UART0_RXD, PULL_NONE),

	/* GPIO_138 - UART_FCH_TX_DEBUG_RX */
	PAD_NF(GPIO_138, UART0_TXD, PULL_NONE),

	/* TPM_SERIRQ# */
	PAD_GPI(GPIO_139, PULL_UP),

	/* BD_ID2 */
	PAD_GPI(GPIO_140, PULL_NONE),

	/* APU_SPI_WP */
	PAD_GPI(GPIO_142, PULL_UP),

	/* BD_ID3 */
	PAD_GPI(GPIO_144, PULL_NONE),
};

const struct soc_amd_stoneyridge_gpio gpio_set_stage_ram[] = {
	/* AGPIO 12 */
	PAD_GPI(GPIO_12, PULL_UP),

	/* TS_EN_SOC (TouchScreen enable GPIO) */
	PAD_GPO(GPIO_13, HIGH),

	/* CAM_PWRON (Camera enable GPIO) */
	PAD_GPO(GPIO_14, HIGH),

	/* APU_BT_ON# */
	PAD_GPO(GPIO_24, HIGH),

	/* DEVSLP1_SSD */
	PAD_NF(GPIO_67, DEVSLP0, PULL_UP),

	/* DEVSLP1_EMMC */
	/* No Connect for now.
	 * {GPIO_70, Function1, FCH_GPIO_PULL_UP_ENABLE | INPUT},
	 */

	/* CAM_LED# */
	PAD_GPO(GPIO_84, HIGH),

	/* TS_RST#  (TouchScreen Reset) */
	PAD_GPO(GPIO_85, HIGH),

	/* WLAN_RST#_AUX */
	PAD_GPO(GPIO_119, HIGH),
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
